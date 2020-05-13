#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "addr.h"
#include "base.h"


void ip4_addr_init_null ( struct ip4_addr_t* const restrict obj ) {
    obj->prefixlen = 0;
    obj->addr      = 0;
}


void ip4_addr_init (
    struct ip4_addr_t* const restrict obj,
    const ip_prefixlen_t prefixlen,
    const ip4_addr_data_t addr
) {
    obj->prefixlen = prefixlen;
    obj->addr      = addr;
}


void ip4_addr_init_child (
    struct ip4_addr_t* const restrict obj,
    const struct ip4_addr_t* const restrict src,
    const bool subaddr_bit_is_set
) {
    obj->prefixlen = src->prefixlen + 1;
    ip4_calc_set_bit_at_prefixpos (
        src->addr, obj->prefixlen, subaddr_bit_is_set, &(obj->addr)
    );
}


void ip4_addr_init_sibling (
    struct ip4_addr_t* const restrict obj,
    const struct ip4_addr_t* const restrict src
) {
    obj->prefixlen = src->prefixlen;
    ip4_calc_flip_bit_at_prefixpos (
        src->addr, obj->prefixlen, &(obj->addr)
    );
}


void ip4_addr_flip_inplace ( struct ip4_addr_t* const restrict obj ) {
    const ip4_addr_data_t old_addr = obj->addr;

    ip4_calc_flip_bit_at_prefixpos ( old_addr, obj->prefixlen, &(obj->addr));
}


bool ip4_addr_bit_is_set_at (
    const struct ip4_addr_t* const restrict obj,
    const ip_prefixlen_t prefixpos
) {
    return ip4_calc_bit_is_set_at_prefixpos ( obj->addr, prefixpos );
}


bool ip4_addr_is_net (
    const struct ip4_addr_t* const restrict obj
) {
    ip4_addr_data_t mask;

    if ( obj->prefixlen > IP4_MAX_PREFIXLEN ) {
        return false;

    } else {
        /* mask == all host bits set to 1 */
        mask = ( 0x1 << (IP4_MAX_PREFIXLEN - obj->prefixlen) ) - 1;

        return ( ((obj->addr) & mask) == 0 );
    }
}
