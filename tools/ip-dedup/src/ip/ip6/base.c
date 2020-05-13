#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stdio.h>  /* snprintf() */

#include "base.h"
#include "../base.h"


static void ip6_addr_data_copy_to_blocks (
    const struct ip6_addr_data* const restrict bits,
    uint16_t* const restrict blocks
);


static void ip6_addr_data_find_longest_zero_seq (
    const uint16_t* const restrict blocks,
    size_t* const restrict zseq_start,
    size_t* const restrict zseq_len
);


static int ip6_addr_data_build_addr_str (
    const uint16_t* const restrict blocks,
    char* const restrict sbuf,
    const size_t fill_zero_start,
    const size_t fill_zero_len
);


static ip_bitpos_t ip6_calc_bitpos ( const ip_prefixlen_t prefixpos ) {
    return ( IP6_MAX_PREFIXLEN - prefixpos );
}

static void ip6_calc_ro_bitpos_in (
    const ip6_addr_data_t* const restrict src,
    const ip_prefixlen_t prefixpos,
    ip6_addr_data_chunk_t* const restrict chunk_out,
    ip_bitpos_t* const restrict bitpos_out,
    bool* const restrict high_out
) {
    ip_bitpos_t bpos;

    bpos = ip6_calc_bitpos(prefixpos);

    if ( bpos < 64 ) {
        *chunk_out  = src->low;
        *bitpos_out = bpos;
        *high_out   = false;

    } else {
        *chunk_out  = src->high;
        *bitpos_out = bpos - 64;
        *high_out   = true;
    }
}


static ip6_addr_data_chunk_t ip6_calc_bit ( const ip_bitpos_t bpos ) {
    ip6_addr_data_chunk_t ret;

    ret = 0x1;
    ret <<= bpos;

    return ret;
}


bool ip6_calc_bit_is_set_at_prefixpos (
    const ip6_addr_data_t* const restrict bits,
    const ip_prefixlen_t prefixpos
) {
    ip6_addr_data_chunk_t chunk;
    ip_bitpos_t bpos;
    bool is_high;

    ip6_calc_ro_bitpos_in ( bits, prefixpos, &chunk, &bpos, &is_high );

    return ( (chunk & ip6_calc_bit(bpos)) != 0 ) ? true : false;
}


void ip6_calc_flip_bit_at_prefixpos (
    const ip6_addr_data_t* const restrict bits,
    const ip_prefixlen_t prefixpos,
    ip6_addr_data_t* const restrict dst
) {
    ip6_addr_data_chunk_t src_chunk;
    ip6_addr_data_chunk_t flipped;
    ip6_addr_data_chunk_t new_chunk;
    ip_bitpos_t bpos;
    bool is_high;

    ip6_calc_ro_bitpos_in ( bits, prefixpos, &src_chunk, &bpos, &is_high );

    flipped   = ip6_calc_bit(bpos);
    new_chunk = (src_chunk ^ flipped);

    if ( is_high ) {
        dst->low  = bits->low;
        dst->high = new_chunk;
    } else {
        dst->low  = new_chunk;
        dst->high = bits->high;
    }
}

void ip6_calc_set_bit_at_prefixpos (
    const ip6_addr_data_t* const restrict bits,
    const ip_prefixlen_t prefixpos,
    bool bit_set,
    ip6_addr_data_t* const restrict dst
) {
    ip6_addr_data_chunk_t src_chunk;
    ip6_addr_data_chunk_t bit_mask;
    ip6_addr_data_chunk_t new_chunk;
    ip_bitpos_t bpos;
    bool is_high;

    ip6_calc_ro_bitpos_in ( bits, prefixpos, &src_chunk, &bpos, &is_high );

    bit_mask  = ip6_calc_bit(bpos);
    new_chunk = ( bit_set ? (src_chunk | bit_mask) : (src_chunk & ~bit_mask) );

    if ( is_high ) {
        dst->low  = bits->low;
        dst->high = new_chunk;
    } else {
        dst->low  = new_chunk;
        dst->high = bits->high;
    }
}


static void ip6_addr_data_copy_to_blocks (
    const struct ip6_addr_data* const restrict bits,
    uint16_t* const restrict blocks
) {
    unsigned k;

    k = 0;
    for ( ; k < 4; k++ ) {
        blocks[k] = (bits->high >> (16 * (4 - k - 1))) & 0xffff;
    }
    for ( ; k < 8; k++ ) {
        blocks[k] = (bits->low >> (16 * (8 - k - 1))) & 0xffff;
    }
}


static void ip6_addr_data_find_longest_zero_seq (
    const uint16_t* const restrict blocks,
    size_t* const restrict zseq_start,
    size_t* const restrict zseq_len
) {
    unsigned k;
    size_t cur_zseq_start;
    size_t cur_zseq_len;

    *zseq_start    = 0;
    *zseq_len      = 0;

    cur_zseq_start = 0;
    cur_zseq_len   = 0;

    for ( k = 0; k < 8; k++ ) {
        if ( blocks[k] != 0 ) {
            if ( cur_zseq_len > *zseq_len ) {
                *zseq_start = cur_zseq_start;
                *zseq_len   = cur_zseq_len;
            }
            cur_zseq_start = k + 1;
            cur_zseq_len   = 0;

        } else if ( cur_zseq_len == 0 ) {
            cur_zseq_start = k;
            cur_zseq_len   = 1;

        } else {
            cur_zseq_len++;
        }
    }

    if ( cur_zseq_len > *zseq_len ) {
        *zseq_start = cur_zseq_start;
        *zseq_len   = cur_zseq_len;
    }
}


static int ip6_addr_data_build_addr_str (
    const uint16_t* const restrict blocks,
    char* const restrict sbuf,
    const size_t fill_zero_start,
    const size_t fill_zero_len
) {
    unsigned k;
    int ret;
    char* s;

    s = sbuf;
    *s = '\0';
    for ( k = 0; k < 8; k++ ) {
        if ( k == fill_zero_start ) {
            if ( k == 0 ) { *s++ = ':'; }
            *s++ = ':';
            k += fill_zero_len - 1; /* k++ */

        } else if ( blocks[k] == 0 ) {
            *s++ = '0';
            if ( k < 7 ) { *s++ = ':'; }

        } else {
            ret = snprintf ( s, 5, ("%" PRIx16), blocks[k] );
            if ( (ret <= 0) || (ret >= 5) ) { return -1; }
            s += ret;
            if ( k < 7 ) { *s++ = ':'; }
        }
    }
    *s = '\0';

    return 0;
}


const char* ip6_addr_data_into_str (
    const struct ip6_addr_data* const restrict bits,
    char* const restrict dst
) {
    uint16_t blocks[8];

    size_t fill_zero_start;
    size_t fill_zero_len;

    if ( dst == NULL ) { return NULL; }

    /* create blocks */
    ip6_addr_data_copy_to_blocks ( bits, blocks );

    /* find longest zero sequence */
    ip6_addr_data_find_longest_zero_seq (
        blocks, &fill_zero_start, &fill_zero_len
    );

    /* create output string */
    if (
        ip6_addr_data_build_addr_str (
            blocks, dst, fill_zero_start, fill_zero_len
        ) != 0
    ) {
        return NULL;
    }

    return dst;
}
