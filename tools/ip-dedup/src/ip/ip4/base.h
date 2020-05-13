#ifndef _HAVE_IP_IP4_BASE_H_
#define _HAVE_IP_IP4_BASE_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>

#include "../base.h"

#define IP4_MAX_PREFIXLEN 32

typedef uint_fast32_t ip4_addr_data_t;

#define IP4_BYTE_FMT_SPEC    PRIuFAST32

#define IP4_BYTE_FMT         "%" IP4_BYTE_FMT_SPEC
#define IP4_ADDR_FMT         IP4_BYTE_FMT "." IP4_BYTE_FMT "." IP4_BYTE_FMT "." IP4_BYTE_FMT
#define IP4_NET_FMT          IP4_ADDR_FMT "/" IP_PREFIXLEN_FMT

/* extract bytes from an IPv4 addr, ip4_block_<POS> for POS in a.b.c.d */
#define ip4_block_a(_addr)      (((_addr) >> 24) & 0xff)
#define ip4_block_b(_addr)      (((_addr) >> 16) & 0xff)
#define ip4_block_c(_addr)      (((_addr) >>  8) & 0xff)
#define ip4_block_d(_addr)      ((_addr) & 0xff)

#define ip4_addr_fmt_args(_addr)  \
    ip4_block_a(_addr), \
    ip4_block_b(_addr), \
    ip4_block_c(_addr), \
    ip4_block_d(_addr)


bool ip4_calc_bit_is_set_at_prefixpos (
    const ip4_addr_data_t bits,
    const ip_prefixlen_t prefixpos
);

void ip4_calc_flip_bit_at_prefixpos (
    const ip4_addr_data_t bits,
    const ip_prefixlen_t prefixpos,
    ip4_addr_data_t* const restrict dst
);

void ip4_calc_set_bit_at_prefixpos (
    const ip4_addr_data_t bits,
    const ip_prefixlen_t prefixpos,
    bool bit_set,
    ip4_addr_data_t* const restrict dst
);

#endif  /* _HAVE_IP_IP4_BASE_H_ */
