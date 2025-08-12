#include "malcolm.h"

// INFO : va_arg and using printf
int ft_assert(int check, const char *format, ...)
{
  va_list list;

  va_start(list, format);
  if (!check)
  {
    fprintf(stderr, "Error : ");
    vfprintf(stderr, format, list);
    if (errno)
      fprintf(stderr, " : %s", strerror(errno));
    fprintf(stderr, "\n");
    va_end(list);
    exit(EXIT_FAILURE);
  }
  va_end(list);
  return (true);
}

void cleanup_handler(int signo)
{
  (void)signo; // Not use signo

  if (data.sockfd)
    close(data.sockfd);
  if (data.pcap_file_fd)
    close(data.pcap_file_fd);
  fprintf(stdout, "Exiting program...\n");
  exit(EXIT_SUCCESS);
}

void print_mac(const char *prefix, const unsigned char mac[6], const char *suffix)
{
  if (prefix)
    printf("%s", prefix);
  printf("%.2X-%.2X-%.2X-%.2X-%.2X-%.2X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  if (suffix)
    printf("%s", suffix);
}

void print_ipv4(const char *prefix, const unsigned char ip[4], const char *suffix)
{
  if (prefix)
    printf("%s", prefix);
  printf("%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);
  if (suffix)
    printf("%s", suffix);
}

void print_ipv6(const char *prefix, const unsigned char ip[16], const char *suffix)
{
  if (prefix)
    printf("%s", prefix);
  printf("%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
         ip[0], ip[1], ip[2], ip[3], ip[4], ip[5], ip[6], ip[7],
         ip[8], ip[9], ip[10], ip[11], ip[12], ip[13], ip[14], ip[15]);
  if (suffix)
    printf("%s", suffix);
}

void print_ether_hdr(const struct ethhdr *eth)
{
  print_mac("\t|Ether Sender Address	: ", eth->h_source, "\n");
  print_mac("\t|Ether Destination Address : ", eth->h_dest, "\n");
  printf("\tEther_prot = %d\n", eth->h_proto);
}

void print_arp_hdr(const struct arp_packet *arp_hdr)
{
  printf("ar_hrd = %d\n", ntohs(arp_hdr->hdr.ar_hrd));
  printf("ar_pro = %d\n", ntohs(arp_hdr->hdr.ar_pro));
  printf("ar_hln = %d\n", arp_hdr->hdr.ar_hln);
  printf("ar_pln = %d\n", arp_hdr->hdr.ar_pln);
  printf("ar_op = %d\n", ntohs(arp_hdr->hdr.ar_op));
  print_mac("\t|-Sender Address : ", arp_hdr->sender_mac, "\n");
  print_ipv4("Sender IP : ", arp_hdr->sender_ip, "\n");
  print_mac("\t|-Targer Address : ", arp_hdr->target_mac, "\n");
  print_ipv4("Target IP : ", arp_hdr->target_ip, "\n");
}

void print_ndp_hdr(const struct ndp_packet *ndp_hdr)
{
  printf("ICMPv6 type = %d\n", ndp_hdr->icmp6_hdr.icmp6_type);
  printf("ICMPv6 code = %d\n", ndp_hdr->icmp6_hdr.icmp6_code);
  printf("ICMPv6 checksum = %d\n", ntohs(ndp_hdr->icmp6_hdr.icmp6_cksum));
  print_ipv6("Target IP : ", ndp_hdr->target_ip, "\n");
  printf("Option type = %d\n", ndp_hdr->option_type);
  printf("Option length = %d\n", ndp_hdr->option_length);
  print_mac("\t|-Target MAC Address : ", ndp_hdr->target_mac, "\n");
}

void print_arp_packet(const unsigned char *buffer)
{
  const struct ethhdr     *eth     = (struct ethhdr *)buffer;
  const struct arp_packet *arp_hdr = (struct arp_packet *)(buffer + sizeof(struct ethhdr));

  print_ether_hdr(eth);
  print_arp_hdr(arp_hdr);
}

void print_ndp_packet(const unsigned char *buffer)
{
  const struct ethhdr         *eth      = (struct ethhdr *)buffer;
  const struct ipv6_ndp_packet *ipv6_ndp = (struct ipv6_ndp_packet *)(buffer + sizeof(struct ethhdr));

  print_ether_hdr(eth);
  printf("IPv6 version = %d\n", (ipv6_ndp->ipv6_hdr.ip6_vfc >> 4));
  printf("IPv6 payload length = %d\n", ntohs(ipv6_ndp->ipv6_hdr.ip6_plen));
  printf("IPv6 next header = %d\n", ipv6_ndp->ipv6_hdr.ip6_nxt);
  print_ipv6("IPv6 source : ", (unsigned char *)&ipv6_ndp->ipv6_hdr.ip6_src, "\n");
  print_ipv6("IPv6 destination : ", (unsigned char *)&ipv6_ndp->ipv6_hdr.ip6_dst, "\n");
  print_ndp_hdr(&ipv6_ndp->ndp);
}
