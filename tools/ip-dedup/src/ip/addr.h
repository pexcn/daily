#ifndef _HAVE_IP_ADDR_H_
#define _HAVE_IP_ADDR_H_

#include <stdlib.h>
#include <stdint.h>

#include "base.h"
#include "ip4/addr.h"
#include "ip6/addr.h"

union _ip_addr_variant_t;
typedef union _ip_addr_variant_t ip_addr_variant_t;


union _ip_addr_variant_t {
    struct ip4_addr_t v4;
    struct ip6_addr_t v6;
};


#endif  /* _HAVE_IP_ADDR_H_ */
