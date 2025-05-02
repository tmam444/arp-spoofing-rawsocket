#include "malcolm.h"

static bool check_ipaddr_memcmp(const void *if_addr, const void *if_netmask, const void *ip, size_t addr_size)
{
  uint8_t masked_if_addr[addr_size];
  uint8_t masked_ip[addr_size];

  for (size_t i = 0; i < addr_size; i++)
  {
    masked_if_addr[i] = ((uint8_t *)if_addr)[i] & ((uint8_t *)if_netmask)[i];
    masked_ip[i]      = ((uint8_t *)ip)[i] & ((uint8_t *)if_netmask)[i];
  }
  return ft_memcmp(masked_if_addr, masked_ip, addr_size) == 0;
}

static int calculate_mask_length(const void *netmask, int size)
{
  int            length     = 0;
  const uint8_t *u8_netmask = netmask;

  for (int i = 0; i < size; i++)
  {
    uint8_t byte = u8_netmask[i];

    while (byte)
    {
      length += byte & 1;
      byte >>= 1;
    }
  }
  return (length);
}

static bool compare_ipv4(struct ifaddrs *cur, const struct ifaddrs *prev)
{
  const struct sockaddr_in *addr    = (struct sockaddr_in *)cur->ifa_addr;
  const struct sockaddr_in *netmask = (struct sockaddr_in *)cur->ifa_netmask;
  bool                      is_local_network, ret = false;

  is_local_network =
      check_ipaddr_memcmp(&addr->sin_addr, &netmask->sin_addr, &data.source_ipv4, sizeof(data.source_ipv4)) &&
      check_ipaddr_memcmp(&addr->sin_addr, &netmask->sin_addr, &data.target_ipv4, sizeof(data.target_ipv4));
  if (is_local_network)
  {
    if (!prev)
      ret = true;
    else
    {
      const struct sockaddr_in *prev_netmask = (struct sockaddr_in *)prev->ifa_netmask;

      if (calculate_mask_length(&prev_netmask->sin_addr.s_addr, sizeof(prev_netmask->sin_addr.s_addr)) <
          calculate_mask_length(&netmask->sin_addr.s_addr, sizeof(netmask->sin_addr.s_addr)))
        ret = true;
    }
  }
  return (ret);
}

static bool compare_ipv6(struct ifaddrs *cur, const struct ifaddrs *prev)
{
  const struct sockaddr_in6 *addr    = (struct sockaddr_in6 *)cur->ifa_addr;
  const struct sockaddr_in6 *netmask = (struct sockaddr_in6 *)cur->ifa_netmask;
  bool                       is_local_network, ret = false;

  is_local_network =
      check_ipaddr_memcmp(&addr->sin6_addr, &netmask->sin6_addr, &data.source_ipv6, sizeof(data.source_ipv6)) &&
      check_ipaddr_memcmp(&addr->sin6_addr, &netmask->sin6_addr, &data.target_ipv6, sizeof(data.target_ipv6));
  if (is_local_network)
  {
    if (!prev)
      ret = true;
    else
    {
      const struct sockaddr_in6 *prev_netmask = (struct sockaddr_in6 *)prev->ifa_netmask;

      if (calculate_mask_length(&prev_netmask->sin6_addr, sizeof(prev_netmask->sin6_addr)) <
          calculate_mask_length(&netmask->sin6_addr, sizeof(netmask->sin6_addr)))
        ret = true;
    }
  }
  return (ret);
}

static bool compare_packet(struct ifaddrs *cur, const struct ifaddrs *prev)
{
  (void)prev;
  return (ft_strncmp(data.if_name, cur->ifa_name, ft_strlen(cur->ifa_name)) == 0);
}

static struct ifaddrs *find_interface(struct ifaddrs *loop, sa_family_t family,
                                      bool (*compare)(struct ifaddrs *, const struct ifaddrs *))
{
  struct ifaddrs *ret = NULL;

  while (loop != NULL)
  {
    if (loop->ifa_addr && loop->ifa_addr->sa_family == family)
    {
      if (compare(loop, ret))
        ret = loop;
    }
    loop = loop->ifa_next;
  }
  if (ret)
    printf("name = %s\n", ret->ifa_name);
  return (ret);
}

void select_interface()
{
  struct ifaddrs *addrs = NULL, *cur;

  ft_assert(getifaddrs(&addrs) == 0, "getifaddrs failed..");
  if (data.ip_type == IPV4)
    cur = find_interface(addrs, AF_INET, compare_ipv4);
  else
    cur = find_interface(addrs, AF_INET6, compare_ipv6);
  if (cur)
  {
    if (data.ip_type == IPV4)
    {
      const struct sockaddr_in *addr = (struct sockaddr_in *)cur->ifa_addr;
      data.my_ipv4                   = addr->sin_addr;
    }
    else
    {
      const struct sockaddr_in6 *addr = (struct sockaddr_in6 *)cur->ifa_addr;
      data.my_ipv6                    = addr->sin6_addr;
    }
    ft_memcpy(data.if_name, cur->ifa_name, ft_strlen(cur->ifa_name));
    data.if_index = if_nametoindex(cur->ifa_name);
    cur           = find_interface(addrs, AF_PACKET, compare_packet);
    if (cur)
    {
      struct sockaddr_ll *addr = (struct sockaddr_ll *)cur->ifa_addr;
      ft_memcpy(data.my_mac, addr->sll_addr, MAC_ADDR_LEN);
    }
  }
  ft_assert(cur != NULL, "Not found proper network interface");
  if (addrs)
    freeifaddrs(addrs);
}
