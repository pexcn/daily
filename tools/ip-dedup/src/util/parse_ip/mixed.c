#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "mixed.h"
#include "common.h"
#include "ip4.h"
#include "ip6.h"

#include "../../ip.h"


static int parse_ip_mixed_guess_type (
    const char* const line, int* const type_result
);

static ip_addr_variant_t _parse_ip_mixed_buf;


int parse_ip_mixed_addr (
    char* const line,
    const size_t slen,
    int* const type_result,
    struct ip4_addr_t* const restrict addr_v4,
    struct ip6_addr_t* const restrict addr_v6
) {
    const char* prefixlen_str;

    *type_result = PARSE_IP_TYPE_NONE;  /* mostly redundant */

    if (
        parse_ip_split_prefixlen ( line, slen, &prefixlen_str ) == NULL
    ) {
        return PARSE_IP_RET_INVALID;
    }

    return parse_ip_mixed_addr_split (
        line, prefixlen_str, type_result, addr_v4, addr_v6
    );
}


int parse_ip_mixed_net_addr (
    char* const line,
    const size_t slen,
    int* const type_result,
    struct ip4_addr_t* const restrict addr_v4,
    struct ip6_addr_t* const restrict addr_v6
) {
    int ret;

    ret = parse_ip_mixed_addr ( line, slen, type_result, addr_v4, addr_v6 );
    if ( ret != PARSE_IP_RET_SUCCESS ) { return ret; }

    /*
     * Check whether parsed IPv4 addr has host bits set.
     * If so, then invalidate result.
     * */
    if ( ((*type_result) & PARSE_IP_TYPE_IPV4) != 0 ) {
        if ( ! ip4_addr_is_net ( addr_v4 ) ) {
            *type_result &= ~PARSE_IP_TYPE_IPV4;
        }
    }

    /* once more for IPv6 */
    if ( ((*type_result) & PARSE_IP_TYPE_IPV6) != 0 ) {
        if ( ! ip6_addr_is_net ( addr_v6 ) ) {
            *type_result &= ~PARSE_IP_TYPE_IPV6;
        }
    }

    /* at least one type should have survived, otherwise return invalid_net */
    if ( ((*type_result) & PARSE_IP_TYPE_BOTH) != 0 ) {
        return PARSE_IP_RET_SUCCESS;
    } else {
        return PARSE_IP_RET_INVALID_NET;
    }
}


int parse_ip_mixed_addr_split (
    char* const addr_str,
    const char* const prefixlen_str,
    int* const type_result,
    struct ip4_addr_t* const restrict addr_v4,
    struct ip6_addr_t* const restrict addr_v6
) {
    int guessed_type;
    int ret_guess;
    int ret_v4;
    int ret_v6;

    *type_result = PARSE_IP_TYPE_NONE;

    ret_guess = parse_ip_mixed_guess_type ( addr_str, &guessed_type );
    if ( ret_guess != PARSE_IP_RET_SUCCESS ) { return ret_guess; }

    /* parse IPv4 */
    if ( (guessed_type & PARSE_IP_TYPE_IPV4) != 0 ) {
        ret_v4 = parse_ip4_addr_split (
            addr_str, prefixlen_str, &(_parse_ip_mixed_buf.v4)
        );

        if ( ret_v4 == PARSE_IP_RET_SUCCESS ) {
            *type_result |= PARSE_IP_TYPE_IPV4;
            *addr_v4      = _parse_ip_mixed_buf.v4;
        }
    } else {
        ret_v4 = PARSE_IP_RET_DID_NOT_TRY;
    }

    /* parse IPv6 */
    if ( (guessed_type & PARSE_IP_TYPE_IPV6) != 0 ) {
        ret_v6 = parse_ip6_addr_split (
            addr_str, prefixlen_str, &(_parse_ip_mixed_buf.v6)
        );

        if ( ret_v6 == PARSE_IP_RET_SUCCESS ) {
            *type_result |= PARSE_IP_TYPE_IPV6;
            *addr_v6      = _parse_ip_mixed_buf.v6;
        }
    } else {
        ret_v6 = PARSE_IP_RET_DID_NOT_TRY;
    }

    /*
     * lower retcode wins.
     * In particular, any severe error (< 0) is returned and
     * one successful parse operation is enough, even for PARSE_IP_TYPE_BOTH.
     * */
    return ( (ret_v4 < ret_v6) ? ret_v4 : ret_v6 );
}


static int parse_ip_mixed_guess_type (
    const char* const line,
    int* const type_result
) {
    const char* s;
    bool digits_seen;

    *type_result = PARSE_IP_TYPE_NONE;

    digits_seen = false;
    s = line;

    /* hex notation starting with 0x could be anything */
    /* FIXME: underlying parser does not handle 0x gracefully */
    if ( (*s == '0') && (*(s + 1) == 'x') ) {
        *type_result = PARSE_IP_TYPE_BOTH;
        return PARSE_IP_RET_SUCCESS;
    }

    for ( ; ; s++ ) {
        switch ( *s ) {
            case '\0':
            case '/':
                /* end of str or prefix starts, check if digits seen in line */
                if ( digits_seen ) {
                    *type_result = PARSE_IP_TYPE_BOTH;
                    return PARSE_IP_RET_SUCCESS;
                } else {
                    return PARSE_IP_RET_INVALID;
                }

            case '.':
                *type_result = PARSE_IP_TYPE_IPV4;
                return PARSE_IP_RET_SUCCESS;

            case ':':
                *type_result = PARSE_IP_TYPE_IPV6;
                return PARSE_IP_RET_SUCCESS;

            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                digits_seen = true;
                break;

            case 'a':
            case 'b':
            case 'c':
            case 'd':
            case 'e':
            case 'f':
            case 'A':
            case 'B':
            case 'C':
            case 'D':
            case 'E':
            case 'F':
                *type_result = PARSE_IP_TYPE_IPV6;
                return PARSE_IP_RET_SUCCESS;

            default:
                return PARSE_IP_RET_INVALID;
        }
    }
}
