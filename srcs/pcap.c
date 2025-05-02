#include "malcolm.h"
#include <fcntl.h>

// Initialize PCAP file
int init_pcap_file(const char *filename)
{
  int fd;

  fd = open(filename, O_CREAT | O_RDWR);
  ft_assert(fd != -1, "Failed to open PCAP file");
  if (DEBUG)
    printf("make pcap file!!\n");
  struct pcap_file_header file_header = {
    .magic_number  = 0xa1b2c3d4, // Magic number for pcap
    .version_major = 2,
    .version_minor = 4,
    .thiszone      = 0,
    .sigfigs       = 0,
    .snaplen       = 65535,
    .network       = 1 // Ethernet
  };
  write(fd, &file_header, sizeof(file_header));
  return fd;
}

// Save a single packet to the PCAP file
void save_packet_to_pcap(int fd, unsigned char *buffer, int buflen)
{
  struct timeval ts;
  gettimeofday(&ts, NULL);

  struct pcap_packet_header pkt_header = {
    .ts_sec = ts.tv_sec, .ts_usec = ts.tv_usec, .incl_len = buflen, .orig_len = buflen
  };
  write(fd, &pkt_header, sizeof(pkt_header));
  write(fd, buffer, buflen);
}
