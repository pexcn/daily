#ifndef _HAVE_IP_IP4_ADDR_H_
#define _HAVE_IP_IP4_ADDR_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "base.h"

struct ip4_addr_t {
    ip_prefixlen_t   prefixlen;
    ip4_addr_data_t  addr;
};

void ip4_addr_init_null ( struct ip4_addr_t* const restrict obj );

void ip4_addr_init (
    struct ip4_addr_t* const restrict obj,
    const ip_prefixlen_t prefixlen,
    const ip4_addr_data_t addr
);

void ip4_addr_init_child (
    struct ip4_addr_t* const restrict obj,
    const struct ip4_addr_t* const restrict src,
    const bool subaddr_bit_is_set
);

void ip4_addr_init_sibling (
    struct ip4_addr_t* const restrict obj,
    const struct ip4_addr_t* const restrict src
);

void ip4_addr_flip_inplace ( struct ip4_addr_t* const restrict obj );

bool ip4_addr_bit_is_set_at (
    const struct ip4_addr_t* const restrict obj,
    const ip_prefixlen_t prefixpos
);

bool ip4_addr_is_net (
    const struct ip4_addr_t* const restrict obj
);

#endif  /* _HAVE_IP_IP4_ADDR_H_ */
