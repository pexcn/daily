#ifndef _HAVE_IP_IP6_ADDR_H_
#define _HAVE_IP_IP6_ADDR_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "base.h"

struct ip6_addr_t {
    ip_prefixlen_t  prefixlen;
    ip6_addr_data_t addr;
};


void ip6_addr_init_null ( struct ip6_addr_t* const restrict obj );

void ip6_addr_init (
    struct ip6_addr_t* const restrict obj,
    const ip_prefixlen_t prefixlen,
    const ip6_addr_data_t* const restrict addr
);

void ip6_addr_init_child (
    struct ip6_addr_t* const restrict obj,
    const struct ip6_addr_t* const restrict src,
    const bool subaddr_bit_is_set
);

void ip6_addr_init_sibling (
    struct ip6_addr_t* const restrict obj,
    const struct ip6_addr_t* const restrict src
);

void ip6_addr_flip_inplace ( struct ip6_addr_t* const restrict obj );

bool ip6_addr_bit_is_set_at (
    const struct ip6_addr_t* const restrict obj,
    const ip_prefixlen_t prefixpos
);

bool ip6_addr_is_net (
    const struct ip6_addr_t* const restrict obj
);

#endif  /* _HAVE_IP_IP6_ADDR_H_ */
