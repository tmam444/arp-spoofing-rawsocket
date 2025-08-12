#include "malcolm.h"
#include <fcntl.h>
#include <sys/uio.h>

// Initialize PCAP file
int init_pcap_file(const char *filename)
{
  int fd;

  fd = open(filename, O_CREAT | O_RDWR, 0644);
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
  if (write(fd, &file_header, sizeof(file_header)) < 0) {
    perror("Failed to write PCAP header");
  }
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
  
  // Optimized: Use writev for atomic write operation
  struct iovec iov[2];
  iov[0].iov_base = &pkt_header;
  iov[0].iov_len = sizeof(pkt_header);
  iov[1].iov_base = buffer;
  iov[1].iov_len = buflen;
  
  if (writev(fd, iov, 2) < 0) {
    perror("Failed to write packet to PCAP");
  }
}
