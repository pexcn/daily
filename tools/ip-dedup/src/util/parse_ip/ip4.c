#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "ip4.h"
#include "common.h"
#include "../../ip/ip4.h"

#define PARSE_IP4_BLOCK_COUNT       4
#define PARSE_IP4_BLOCK_BASE        8
#define PARSE_IP4_INPUT_BLOCK_BASE  10

typedef uint_fast8_t    parse_ip4_block_t;

struct parse_ip4_data {
    size_t num_blocks;

    parse_ip4_block_t blocks[PARSE_IP4_BLOCK_COUNT];
    ip_prefixlen_t prefixlen;
};

static struct parse_ip4_data _parse_ip4_buf;


static void parse_ip4_data_init (
    struct parse_ip4_data* const restrict obj
);

static void parse_ip4_data_append (
    struct parse_ip4_data* const restrict obj,
    const parse_ip4_block_t block
);



__attribute__((warn_unused_result))
static int parse_ip4_addr_split_read_hex (
    struct parse_ip4_data* const restrict buf,
    char* const restrict addr_str
);

__attribute__((warn_unused_result))
static int parse_ip4_addr_split_read_blocks (
    struct parse_ip4_data* const restrict buf,
    char* const restrict addr_str
);

__attribute__((warn_unused_result))
static int parse_ip4_addr_split_read_prefixlen (
    struct parse_ip4_data* const restrict buf,
    const char* const restrict prefixlen_str
);


static void parse_ip4_addr_split_make_addr (
    struct parse_ip4_data* const restrict buf,
    struct ip4_addr_t* const restrict addr
);


int parse_ip4_addr (
   char* const restrict line,
   const size_t slen,
   struct ip4_addr_t* const restrict addr
) {
    const char* prefixlen_str;

    if (
        parse_ip_split_prefixlen ( line, slen, &prefixlen_str ) == NULL
    ) {
        return PARSE_IP_RET_INVALID;
    }

    return parse_ip4_addr_split ( line, prefixlen_str, addr );
}


int parse_ip4_net_addr (
   char* const restrict line,
   const size_t slen,
   struct ip4_addr_t* const restrict addr
) {
    int ret;

    ret = parse_ip4_addr ( line, slen, addr );
    if ( ret != PARSE_IP_RET_SUCCESS ) { return ret; }

    return ( ip4_addr_is_net(addr) ? PARSE_IP_RET_SUCCESS : PARSE_IP_RET_INVALID_NET );
}


int parse_ip4_addr_split (
    char* const addr_str,
    const char* const prefixlen_str,
    struct ip4_addr_t* const restrict addr
) {
    int ret;

    parse_ip4_data_init ( &_parse_ip4_buf );

    ip4_addr_init_null ( addr );

    if ( (addr_str == NULL) || (*addr_str == '\0') ) {
        ret = PARSE_IP_RET_INVALID;

    } else if ( (*addr_str == '0') && (*(addr_str + 1) == 'x') ) {
        ret = parse_ip4_addr_split_read_hex ( &_parse_ip4_buf, (addr_str + 2) );

    } else {
        ret = parse_ip4_addr_split_read_blocks ( &_parse_ip4_buf, addr_str );
    }

    if ( ret == PARSE_IP_RET_SUCCESS ) {
        ret = parse_ip4_addr_split_read_prefixlen (
            &_parse_ip4_buf, prefixlen_str
        );

        if ( ret == PARSE_IP_RET_SUCCESS ) {
            parse_ip4_addr_split_make_addr ( &_parse_ip4_buf, addr );
        }
    }

    return ret;
}


static void parse_ip4_data_init (
    struct parse_ip4_data* const restrict obj
) {
    size_t k;

    obj->num_blocks = 0;

    for ( k = 0; k < PARSE_IP4_BLOCK_COUNT; k++ ) {
        obj->blocks[k] = 0;
    }

    obj->prefixlen = 0;
}


static void parse_ip4_data_append (
    struct parse_ip4_data* const restrict obj,
    const parse_ip4_block_t block
) {
    obj->blocks[(obj->num_blocks)++] = block;
}


static void parse_ip4_addr_split_make_addr (
    struct parse_ip4_data* const restrict buf,
    struct ip4_addr_t* const restrict addr
) {
    static const size_t max_block_count = PARSE_IP4_BLOCK_COUNT;
    static const parse_ip4_block_t block_base = PARSE_IP4_BLOCK_BASE;

    size_t k;
    ip4_addr_data_t chunk;

    for ( k = 0; k < buf->num_blocks; k++ ) {
        chunk = buf->blocks[k];
        /*
         * shift by
         * idx | cnt * block_base = val
         * 0 -> 3 :: 24
         * 1 -> 2 :: 16
         * 2 -> 1 ::  8
         * 1 -> 0 ::  0
         *
         * k -> max_block_count - k - 1
         * */
        chunk <<= (max_block_count - k - 1) * block_base;

        addr->addr |= chunk;
    }

    addr->prefixlen = buf->prefixlen;
}


static int parse_ip4_addr_split_read_hex (
    __attribute__((unused)) struct parse_ip4_data* const restrict buf,
    __attribute__((unused)) char* const restrict addr_str
) {
    return PARSE_IP_RET_NOT_IMPLEMENTED;
}


static int parse_ip4_addr_split_read_blocks (
    struct parse_ip4_data* const restrict buf,
    char* const restrict addr_str
) {
    static const size_t max_block_count = PARSE_IP4_BLOCK_COUNT;
    static const int input_block_base   = PARSE_IP4_INPUT_BLOCK_BASE;
    static const char block_sep         = '.';

    parse_ip4_block_t block;
    char* block_str;
    char* addr_str_rem;

    addr_str_rem = addr_str;
    while ( (addr_str_rem != NULL) && (*addr_str_rem != '\0') ) {
        if ( buf->num_blocks >= max_block_count ) {
            /* too many blocks in addr_str */
            return PARSE_IP_RET_INVALID;
        }

        block_str = addr_str_rem;

        /* find end of block */
        addr_str_rem = strchr ( block_str, block_sep );
        if ( addr_str_rem != NULL ) {
            /* there is a next block, insert end marker */
            *addr_str_rem = '\0';
            addr_str_rem++;

            if ( *addr_str_rem == '\0' ) {
                /* "." at end of string
                 * allowed IFF at least one block is missing in input
                 * */
                if ( (max_block_count - (buf->num_blocks)) < 2 ) {
                    return PARSE_IP_RET_INVALID;
                }

            } else if ( *addr_str_rem == block_sep ) {
                /* ".." */
                return PARSE_IP_RET_INVALID;
            }
        } /* otherwise, assume that there's a terminating null char */

        if ( *block_str == '\0' ) {
            /* then empty block, block_str pointed to a "." before */
            /* this is an error for IPv4 */
            return PARSE_IP_RET_INVALID;

        } else if (
            parse_ip_read_octet ( block_str, input_block_base, &block ) != 0
        ) {
            return PARSE_IP_RET_INVALID;
        }

        parse_ip4_data_append ( buf, block );
    }

    if ( buf->num_blocks == 0 ) { return PARSE_IP_RET_INVALID; }

    return PARSE_IP_RET_SUCCESS;
}


static int parse_ip4_addr_split_read_prefixlen (
    struct parse_ip4_data* const restrict buf,
    const char* const restrict prefixlen_str
) {
    static const ip_prefixlen_t max_prefixlen = IP4_MAX_PREFIXLEN;
    static const ip_prefixlen_t block_base    = PARSE_IP4_BLOCK_BASE;
    static const int input_pfx_base           = 10;

    /* parse prefixlen */
    if ( prefixlen_str == NULL ) {
        /* automatic prefixlen: num_blocks * block_base */
        buf->prefixlen = buf->num_blocks * block_base;
        return PARSE_IP_RET_SUCCESS;

    } else if (
        ( parse_ip_read_octet ( prefixlen_str, input_pfx_base, &(buf->prefixlen) ) != 0 )
        || ( (buf->prefixlen) > max_prefixlen )
    ) {
        return PARSE_IP_RET_INVALID;

    } else {
        return PARSE_IP_RET_SUCCESS;
    }
}
