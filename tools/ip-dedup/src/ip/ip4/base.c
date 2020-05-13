#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "base.h"

static ip_bitpos_t ip4_calc_bitpos ( const ip_prefixlen_t prefixpos ) {
    return IP4_MAX_PREFIXLEN - prefixpos;
}


static ip4_addr_data_t ip4_calc_bit ( const ip_bitpos_t bpos ) {
    ip4_addr_data_t ret;

    ret = 0x1;
    ret <<= bpos;

    return ret;
}

static ip4_addr_data_t ip4_calc_bit_at_prefixpos (
    const ip_prefixlen_t prefixpos
) {
    return ip4_calc_bit (  ip4_calc_bitpos(prefixpos) );
}


static ip4_addr_data_t ip4_calc_get_bit_at_prefixpos (
    const ip4_addr_data_t bits,
    const ip_prefixlen_t prefixpos
) {
    return ( bits & (ip4_calc_bit_at_prefixpos(prefixpos)) );
}


bool ip4_calc_bit_is_set_at_prefixpos (
    const ip4_addr_data_t bits,
    const ip_prefixlen_t prefixpos
) {
    return ( ip4_calc_get_bit_at_prefixpos(bits, prefixpos) != 0 );
}


void ip4_calc_flip_bit_at_prefixpos (
    const ip4_addr_data_t bits,
    const ip_prefixlen_t prefixpos,
    ip4_addr_data_t* const restrict dst
) {
    *dst = ( bits ^ ip4_calc_bit_at_prefixpos(prefixpos) );
}


void ip4_calc_set_bit_at_prefixpos (
    const ip4_addr_data_t bits,
    const ip_prefixlen_t prefixpos,
    bool bit_set,
    ip4_addr_data_t* const restrict dst
) {
    ip4_addr_data_t bit_mask = ip4_calc_bit_at_prefixpos ( prefixpos );

    *dst = ( bit_set ? (bits | bit_mask) : (bits & ~bit_mask) );
}
