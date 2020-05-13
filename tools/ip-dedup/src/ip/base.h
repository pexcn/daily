#ifndef _HAVE_IP_BASE_H_
#define _HAVE_IP_BASE_H_

#include <stdlib.h>
#include <stdint.h>

/** IP address type */
enum {
    IP_TYPE_UNDEF = 0,
    IP_TYPE_IPV4,   /* AF_INET */
    IP_TYPE_IPV6    /* AF_INET6 */
};

typedef uint_fast8_t ip_prefixlen_t;
typedef uint_fast8_t ip_bitpos_t;

#define IP_PREFIXLEN_FMT_SPEC   PRIuFAST8
#define IP_PREFIXLEN_FMT        "%" IP_PREFIXLEN_FMT_SPEC


#endif  /* _HAVE_IP_BASE_H_ */
