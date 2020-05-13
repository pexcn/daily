#ifndef _HAVE_IP_IP6_BASE_H_
#define _HAVE_IP_IP6_BASE_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>

#include "../base.h"

#define IP6_MAX_PREFIXLEN 128
#define IP6_DATA_CHUNK_SIZE 64

/*
 * worst-case addr str size:
 *   8 blocks w/ 0..4 chars
 *   7 colon separator
 *   1 terminating null
 *
 * (the "::" case will fit in this buffer
 * since at least one block will be missing)
 * */
#define IP6_ADDR_STR_BUF_SIZE   ( (8 * 4) + 7 + 1 )

/* split addr in two 64bit chunks -- not using __uint128 */
typedef uint_fast64_t ip6_addr_data_chunk_t;

struct ip6_addr_data {
    ip6_addr_data_chunk_t high;
    ip6_addr_data_chunk_t low;
};
typedef struct ip6_addr_data ip6_addr_data_t;

#define IP6_BLOCK_FMT_SPEC      PRIxFAST64
#define IP6_BLOCK_FMT           "%04" IP6_BLOCK_FMT_SPEC

#define IP6_ADDR_FMT  \
        IP6_BLOCK_FMT \
    ":" IP6_BLOCK_FMT \
    ":" IP6_BLOCK_FMT \
    ":" IP6_BLOCK_FMT \
    ":" IP6_BLOCK_FMT \
    ":" IP6_BLOCK_FMT \
    ":" IP6_BLOCK_FMT \
    ":" IP6_BLOCK_FMT \

#define IP6_NET_FMT             IP6_ADDR_FMT "/" IP_PREFIXLEN_FMT

/* extract bytes from an IPv6 addr,
 * ip6_block_<POS> for POS in a:b:c:d:e:f:g:h */
#define ip6_block_a(_addr)      (((_addr).high >> 48) & 0xffff)
#define ip6_block_b(_addr)      (((_addr).high >> 32) & 0xffff)
#define ip6_block_c(_addr)      (((_addr).high >> 16) & 0xffff)
#define ip6_block_d(_addr)      (((_addr).high      ) & 0xffff)
#define ip6_block_e(_addr)      (((_addr).low  >> 46) & 0xffff)
#define ip6_block_f(_addr)      (((_addr).low  >> 32) & 0xffff)
#define ip6_block_g(_addr)      (((_addr).low  >> 16) & 0xffff)
#define ip6_block_h(_addr)      (((_addr).low       ) & 0xffff)

#define ip6_addr_fmt_args(_addr)  \
    ip6_block_a(_addr), \
    ip6_block_b(_addr), \
    ip6_block_c(_addr), \
    ip6_block_d(_addr), \
    ip6_block_e(_addr), \
    ip6_block_f(_addr), \
    ip6_block_g(_addr), \
    ip6_block_h(_addr)

bool ip6_calc_bit_is_set_at_prefixpos (
    const ip6_addr_data_t* const restrict bits,
    const ip_prefixlen_t prefixpos
);

void ip6_calc_flip_bit_at_prefixpos (
    const ip6_addr_data_t* const restrict bits,
    const ip_prefixlen_t prefixpos,
    ip6_addr_data_t* const restrict dst
);

void ip6_calc_set_bit_at_prefixpos (
    const ip6_addr_data_t* const restrict bits,
    const ip_prefixlen_t prefixpos,
    bool bit_set,
    ip6_addr_data_t* const restrict dst
);


const char* ip6_addr_data_into_str (
    const struct ip6_addr_data* const restrict bits,
    char* const restrict dst
);

#endif  /* _HAVE_IP_IP6_BASE_H_ */
