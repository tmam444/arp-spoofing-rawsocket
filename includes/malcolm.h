/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malcolm.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chulee <chulee@student.42seoul.kr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/15 15:38:16 by chulee            #+#    #+#             */
/*   Updated: 2024/10/30 07:12:52 by chulee           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MALCOLM_H
#define MALCOLM_H

#include "../libft/libft.h"
#include <arpa/inet.h>
#include <errno.h>
#include <ifaddrs.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <netinet/icmp6.h>
#include <netinet/if_ether.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <pthread.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define DEBUG        0
#define ETH_MIN      60
#define MAC_ADDR_LEN 6
#define IP_ADDR_LEN  4
#define BUFFER_SIZE  65535

// PCAP file header structure
struct pcap_file_header
{
    unsigned int   magic_number;
    unsigned short version_major;
    unsigned short version_minor;
    int            thiszone; // GMT to local correction
    unsigned int   sigfigs;
    unsigned int   snaplen;  // Max length of captured packets
    unsigned int   network;  // Data link type (Ethernet in this case)
};

// PCAP packet header structure
struct pcap_packet_header
{
    unsigned int ts_sec;   // Timestamp seconds
    unsigned int ts_usec;  // Timestamp microseconds
    unsigned int incl_len; // Length of packet data in the file
    unsigned int orig_len; // Actual length of the packet
};

typedef enum
{
  UNKNOWN,
  IPV4,
  IPV6
} IP_TYPE;

// INFO : in_addr 로 바꿔보기? -> 완료
// TODO : ip variable -> union 으로 바꿔보기
struct mitm
{
    bool               spoofing;
    int                sockfd;
    socklen_t          socklen;
    char               if_name[IFNAMSIZ];
    int                if_index;
    struct in_addr     my_ipv4;
    struct in6_addr    my_ipv6;
    unsigned char      my_mac[MAC_ADDR_LEN];
    struct in_addr     source_ipv4;
    struct in6_addr    source_ipv6;
    unsigned char      source_mac[MAC_ADDR_LEN];
    struct in_addr     target_ipv4;
    struct in6_addr    target_ipv6;
    unsigned char      target_mac[MAC_ADDR_LEN];
    struct in_addr     gw_ipv4;
    unsigned char      gw_mac[MAC_ADDR_LEN];
    struct sockaddr_ll saddr;
    IP_TYPE            ip_type;
    int                pcap_file_fd;
};

struct thread_arg
{
    unsigned char *buffer;
    unsigned char  mac[MAC_ADDR_LEN];
    int            buflen;
    __be16         protocol;
};

struct arp_packet
{
    struct arphdr hdr;           // ARP 헤더
    unsigned char sender_mac[6]; // 송신자 MAC 주소
    unsigned char sender_ip[4];  // 송신자 IP 주소
    unsigned char target_mac[6]; // 대상 MAC 주소
    unsigned char target_ip[4];  // 대상 IP 주소
};

extern struct mitm data;

// interface
void select_interface();

// utils
int  ft_assert(int check, const char *format, ...);
void cleanup_handler(int signo);
void print_arp_packet(const unsigned char *buffer);
void print_mac(const char *prefix, const unsigned char mac[6], const char *suffix);
void print_ipv4(const char *prefix, const unsigned char ip[4], const char *suffix);
void print_ether_hdr(const struct ethhdr *eth);
void print_arp_hdr(const struct arp_packet *arp_hdr);

// pcap file
int  init_pcap_file(const char *filename);
void save_packet_to_pcap(int fd, unsigned char *buffer, int buflen);

// spoof gateway
void send_gateway_spoofing_packet(unsigned char *buffer, int buflen);
void send_gateway_arp_request_packet(unsigned char *buffer, int buflen);
void make_gateway_arp_packet(unsigned char *buffer);

// send thread func
void *send_fake_arp_reply(void *arg);

#endif
