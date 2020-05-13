#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "ip6.h"
#include "common.h"
#include "../../ip/ip6.h"

#define PARSE_IP6_BLOCK_COUNT       8
#define PARSE_IP6_BLOCK_BASE        16
#define PARSE_IP6_INPUT_BLOCK_BASE  16

typedef uint_fast16_t   parse_ip6_block_t;

struct parse_ip6_data {
    size_t num_left_blocks;
    size_t num_right_blocks;
    size_t num_blocks;  /* redundant, for simpler bounds checking */

    size_t* p_num_lr_blocks;

    parse_ip6_block_t blocks[PARSE_IP6_BLOCK_COUNT];
    ip_prefixlen_t prefixlen;
};

static struct parse_ip6_data _parse_ip6_buf;


static void parse_ip6_data_init (
    struct parse_ip6_data* const restrict obj
);

static void parse_ip6_data_append (
    struct parse_ip6_data* const restrict obj,
    const parse_ip6_block_t block
);



__attribute__((warn_unused_result))
static int parse_ip6_data_switch_to_right (
    struct parse_ip6_data* const restrict obj
);

static bool parse_ip6_data_has_double_colon (
    struct parse_ip6_data* const restrict obj
);


__attribute__((warn_unused_result))
static int parse_ip6_addr_split_read_hex (
    struct parse_ip6_data* const restrict buf,
    char* const restrict addr_str
);

__attribute__((warn_unused_result))
static int parse_ip6_addr_split_read_blocks (
    struct parse_ip6_data* const restrict buf,
    char* const restrict addr_str
);

__attribute__((warn_unused_result))
static int parse_ip6_addr_split_read_prefixlen (
    struct parse_ip6_data* const restrict buf,
    const char* const restrict prefixlen_str
);


static void parse_ip6_addr_split_make_addr (
    struct parse_ip6_data* const restrict buf,
    struct ip6_addr_t* const restrict addr
);



int parse_ip6_addr (
   char* const restrict line,
   const size_t slen,
   struct ip6_addr_t* const restrict addr
) {
    const char* prefixlen_str;

    if (
        parse_ip_split_prefixlen ( line, slen, &prefixlen_str ) == NULL
    ) {
        return PARSE_IP_RET_INVALID;
    }

    return parse_ip6_addr_split ( line, prefixlen_str, addr );
}


int parse_ip6_net_addr (
   char* const restrict line,
   const size_t slen,
   struct ip6_addr_t* const restrict addr
) {
    int ret;

    ret = parse_ip6_addr ( line, slen, addr );
    if ( ret != PARSE_IP_RET_SUCCESS ) { return ret; }

    return ( ip6_addr_is_net(addr) ? PARSE_IP_RET_SUCCESS : PARSE_IP_RET_INVALID_NET );
}


int parse_ip6_addr_split (
    char* const addr_str,
    const char* const prefixlen_str,
    struct ip6_addr_t* const restrict addr
) {
    int ret;

    parse_ip6_data_init ( &_parse_ip6_buf );

    ip6_addr_init_null ( addr );

    if ( (addr_str == NULL) || (*addr_str == '\0') ) {
        ret = PARSE_IP_RET_INVALID;

    } else if ( (*addr_str == '0') && (*(addr_str + 1) == 'x') ) {
        ret = parse_ip6_addr_split_read_hex ( &_parse_ip6_buf, (addr_str + 2) );

    } else {
        ret = parse_ip6_addr_split_read_blocks ( &_parse_ip6_buf, addr_str );
    }

    if ( ret == PARSE_IP_RET_SUCCESS ) {
        ret = parse_ip6_addr_split_read_prefixlen (
            &_parse_ip6_buf, prefixlen_str
        );

        if ( ret == PARSE_IP_RET_SUCCESS ) {
            parse_ip6_addr_split_make_addr ( &_parse_ip6_buf, addr );
        }
    }

    return ret;
}



static void parse_ip6_data_init ( struct parse_ip6_data* const restrict obj ) {
    size_t k;

    obj->num_left_blocks = 0;
    obj->num_right_blocks = 0;
    obj->num_blocks = 0;
    obj->p_num_lr_blocks = &(obj->num_left_blocks);

    for ( k = 0; k < PARSE_IP6_BLOCK_COUNT; k++ ) {
        obj->blocks[k] = 0;
    }

    obj->prefixlen = 0;
}


static void parse_ip6_data_append (
    struct parse_ip6_data* const restrict obj,
    const parse_ip6_block_t block
) {
    obj->blocks[(obj->num_blocks)++] = block;
    (*(obj->p_num_lr_blocks))++;
}


static int parse_ip6_data_switch_to_right (
    struct parse_ip6_data* const restrict obj
) {
    if ( obj->p_num_lr_blocks == &(obj->num_left_blocks) ) {
        obj->p_num_lr_blocks = &(obj->num_right_blocks);
        return 0;
    } else {
        return -1;
    }
}


static bool parse_ip6_data_has_double_colon (
    struct parse_ip6_data* const restrict obj
) {
    return ( obj->p_num_lr_blocks != &(obj->num_left_blocks) ) ? true : false;
}


#define parse_ip6_addr_split_make_addr__consume(_num_consume)  \
    do { \
        const size_t up_to = (_num_consume); \
        \
        size_t k; \
        ip6_addr_data_chunk_t chunk; \
        \
        for ( k = 0; k < up_to; k++ ) { \
            num_blocks_remaining--; \
            \
            chunk = buf->blocks[arr_idx++]; \
            chunk <<= (num_blocks_remaining % 4) * block_base; \
            \
            if ( num_blocks_remaining < 4 ) { \
                addr->addr.low |= chunk; \
            } else { \
                addr->addr.high |= chunk; \
            } \
        } \
    } while (0)



static void parse_ip6_addr_split_make_addr (
    struct parse_ip6_data* const restrict buf,
    struct ip6_addr_t* const restrict addr
) {
    static const parse_ip6_block_t block_base = PARSE_IP6_BLOCK_BASE;

    size_t num_blocks_remaining;
    size_t arr_idx;

    num_blocks_remaining = PARSE_IP6_BLOCK_COUNT;

    arr_idx = 0; /* == loop index */
    parse_ip6_addr_split_make_addr__consume ( buf->num_left_blocks );


    if ( buf->num_right_blocks > 0 ) {
        num_blocks_remaining = buf->num_right_blocks;

        /* arr_idx = num_left_blocks + loop index */
        parse_ip6_addr_split_make_addr__consume ( buf->num_right_blocks );
    }

    addr->prefixlen = buf->prefixlen;
}

#undef parse_ip6_addr_split_make_addr__consume


static int parse_ip6_addr_split_read_hex (
    __attribute__((unused)) struct parse_ip6_data* const restrict buf,
    __attribute__((unused)) char* const restrict addr_str
) {
    return PARSE_IP_RET_NOT_IMPLEMENTED;
}


static int parse_ip6_addr_split_read_blocks (
    struct parse_ip6_data* const restrict buf,
    char* const restrict addr_str
) {
    static const size_t max_block_count  = PARSE_IP6_BLOCK_COUNT;
    static const int    input_block_base = PARSE_IP6_INPUT_BLOCK_BASE;
    static const char   block_sep        = ':';

    parse_ip6_block_t block;
    bool switch_to_rblocks;
    char* block_str;
    char* addr_str_rem;

    addr_str_rem = addr_str;
    while ( (addr_str_rem != NULL) && (*addr_str_rem != '\0') ) {
        if ( buf->num_blocks >= max_block_count ) {
            /* too many blocks in addr_str */
            return PARSE_IP_RET_INVALID;
        }

        switch_to_rblocks = false;
        block_str = addr_str_rem;

        /* find end of block */
        addr_str_rem = strchr ( block_str, block_sep );
        if ( addr_str_rem != NULL ) {
            /* there is a next block, insert end marker */
            *addr_str_rem = '\0';
            addr_str_rem++;

            if ( *addr_str_rem == '\0' ) {
                /* ":" at end of string -> error */
                return PARSE_IP_RET_INVALID;

            } else if ( *addr_str_rem == block_sep ) {
                /* "::" */
                addr_str_rem++;
                if ( *addr_str_rem == block_sep ) {
                    /* ":::" */
                    return PARSE_IP_RET_INVALID;
                } /* else if ( *addr_str_rem == '\0' ) -- handled by while-loop condition */

                switch_to_rblocks = true;
            }
        } /* otherwise, assume that there's a terminating null char */

        /* could write to buf->blocks directly... */
        if ( *block_str == '\0' ) {
            /* then empty block, block_str pointed to a ":" before */
            /* this can only occur on addr_str begin / end */
            block = 0;
        } else if (
            (parse_ip_read_double_octet ( block_str, input_block_base, &block ) != 0)
        ) {
            return PARSE_IP_RET_INVALID;
        }

        parse_ip6_data_append ( buf, block );

        if ( switch_to_rblocks ) {
            if ( parse_ip6_data_switch_to_right ( buf ) != 0 ) {
                /* "::" seen for the second time */
                return PARSE_IP_RET_INVALID;
            }
        }
    }

    if ( buf->num_blocks == 0 ) {
        return PARSE_IP_RET_INVALID;
    }

    if ( parse_ip6_data_has_double_colon ( buf ) ) {
        /* there should be at least one block missing when "::" was specified */
        if ( buf->num_blocks >= max_block_count ) {
            return PARSE_IP_RET_INVALID;
        }
    }

    return PARSE_IP_RET_SUCCESS;
}

static int parse_ip6_addr_split_read_prefixlen (
    struct parse_ip6_data* const restrict buf,
    const char* const restrict prefixlen_str
) {
    static const ip_prefixlen_t max_prefixlen   = IP6_MAX_PREFIXLEN;
    static const ip_prefixlen_t block_base      = PARSE_IP6_BLOCK_BASE;
    static const int            input_pfx_base  = 10;

    /* parse prefixlen */
    if ( prefixlen_str == NULL ) {
        /* automatic prefixlen:
         * - num_right_blocks > 0:  max_prefixlen
         * - else:                  num_blocks * block_base
         *                          (and num_blocks == num_left_blocks, > 0)
         * */
        if ( buf->num_right_blocks > 0 ) {
            buf->prefixlen = max_prefixlen;
        } else {
            buf->prefixlen = buf->num_blocks * block_base;
        }

    } else if (
        ( parse_ip_read_octet ( prefixlen_str, input_pfx_base, &(buf->prefixlen) ) != 0 )
        || ( (buf->prefixlen) > max_prefixlen )
    ) {
        return PARSE_IP_RET_INVALID;
    }

    return PARSE_IP_RET_SUCCESS;
}
