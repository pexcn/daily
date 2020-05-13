#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "addr.h"
#include "base.h"


void ip6_addr_init_null ( struct ip6_addr_t* const restrict obj ) {
    obj->prefixlen = 0;
    obj->addr.low  = 0;
    obj->addr.high = 0;
}


void ip6_addr_init (
    struct ip6_addr_t* const restrict obj,
    const ip_prefixlen_t prefixlen,
    const ip6_addr_data_t* const restrict addr
) {
    obj->prefixlen = prefixlen;
    obj->addr.low  = addr->low;
    obj->addr.high = addr->high;
}


void ip6_addr_init_child (
    struct ip6_addr_t* const restrict obj,
    const struct ip6_addr_t* const restrict src,
    const bool subaddr_bit_is_set
) {
    obj->prefixlen = src->prefixlen + 1;
    ip6_calc_set_bit_at_prefixpos (
        &(src->addr), obj->prefixlen, subaddr_bit_is_set, &(obj->addr)
    );
}


void ip6_addr_init_sibling (
    struct ip6_addr_t* const restrict obj,
    const struct ip6_addr_t* const restrict src
) {
    obj->prefixlen = src->prefixlen;
    ip6_calc_flip_bit_at_prefixpos (
        &(src->addr), obj->prefixlen, &(obj->addr)
    );
}


void ip6_addr_flip_inplace ( struct ip6_addr_t* const restrict obj ) {
    const ip6_addr_data_t old_addr = {
        .low  = obj->addr.low,
        .high = obj->addr.high
    };

    ip6_calc_flip_bit_at_prefixpos ( &old_addr, obj->prefixlen, &(obj->addr));
}


bool ip6_addr_bit_is_set_at (
    const struct ip6_addr_t* const restrict obj,
    const ip_prefixlen_t prefixpos
) {
    return ip6_calc_bit_is_set_at_prefixpos ( &(obj->addr), prefixpos );
}


bool ip6_addr_is_net (
    const struct ip6_addr_t* const restrict obj
) {
#define ip6_addr_is_net__set_mask(_exp)  \
    do { \
        mask = ( ((ip6_addr_data_chunk_t) 0x1) << (_exp) ) - 1; \
    } while (0)


    ip6_addr_data_chunk_t mask;

    if ( obj->prefixlen > IP6_MAX_PREFIXLEN ) {
        return false;

    } else if ( obj->prefixlen > IP6_DATA_CHUNK_SIZE ) {
        /* ignore bits 0..63, check 64..127 */
        ip6_addr_is_net__set_mask ( IP6_MAX_PREFIXLEN - obj->prefixlen );

        return ( (obj->addr.low & mask) == 0 );

    } else {
        /* mask = all host bits set in high addr part, lower must be all-zero */
        ip6_addr_is_net__set_mask ( IP6_DATA_CHUNK_SIZE - obj->prefixlen);

        return ( (obj->addr.low == 0) && (((obj->addr.high) & mask) == 0) );
    }

#undef ip6_addr_is_net__set_mask
}
