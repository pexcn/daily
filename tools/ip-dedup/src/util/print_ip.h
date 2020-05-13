#ifndef _HAVE_UTIL_PRINT_IP_H_
#define _HAVE_UTIL_PRINT_IP_H_

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "../ip.h"

/* IPv4 */
#define fprint_ip4_addr_data(_stream, _addr_data) \
    fprintf(_stream, (IP4_ADDR_FMT "\n"), ip4_addr_fmt_args(addr_data))

#define fprint_ip4_addr(_stream, _addr)  \
    fprintf(_stream, (IP4_ADDR_FMT "\n"), ip4_addr_fmt_args((_addr)->addr))

#define fprint_ip4_net(_stream, _addr)  \
    fprintf(_stream, (IP4_NET_FMT "\n"), ip4_addr_fmt_args((_addr)->addr), (_addr)->prefixlen)


/* IPv6 */
#define fprint_ip6_addr_data(_stream, _addr_data)  \
    fprintf(_stream, (IP6_ADDR_FMT "\n"), ip6_addr_fmt_args(addr_data))

#define fprint_ip6_addr(_stream, _addr)  \
    fprintf(_stream, (IP6_ADDR_FMT "\n"), ip6_addr_fmt_args((_addr)->addr))

#define fprint_ip6_net(_stream, _addr)  \
    fprintf(_stream, (IP6_NET_FMT "\n"), ip6_addr_fmt_args((_addr)->addr), (_addr)->prefixlen)


#endif  /* _HAVE_UTIL_PRINT_IP_H_ */
