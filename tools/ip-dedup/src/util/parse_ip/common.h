#ifndef _HAVE_UTIL_PARSE_IP_COMMON_H_
#define _HAVE_UTIL_PARSE_IP_COMMON_H_

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * parse_ip() return codes
 * - FAIL:             critical failure (e.g. malloc())
 *                     any negative value should be treated this way
 * - SUCCESS:          input was a valid address/network
 * - INVALID:          input was garbage
 * - DID_NOT_TRY:      input didn't look like an address of type _,
 *                     this can only occur for ip_parse_mixed()
 * - NOT_IMPLEMENTED:  missing functionality
 *
 * */
enum {
    PARSE_IP_RET_BAD_INFILE_DUP  = -4,
    PARSE_IP_RET_BAD_INFILE      = -3,
    PARSE_IP_RET_READ_ERR        = -2,
    PARSE_IP_RET_FAIL            = -1,
    PARSE_IP_RET_SUCCESS         = 0,
    PARSE_IP_RET_INVALID,
    PARSE_IP_RET_INVALID_NET,
    PARSE_IP_RET_DID_NOT_TRY,
    PARSE_IP_RET_NOT_IMPLEMENTED,
    PARSE_IP_RET_EOF,

    PARSE_IP_RET__NEXT_FREE
};


enum {
    PARSE_IP_TYPE_NONE  = 0x0,
    PARSE_IP_TYPE_IPV4  = 0x1,
    PARSE_IP_TYPE_IPV6  = 0x2,
    PARSE_IP_TYPE_BOTH  = (PARSE_IP_TYPE_IPV4 | PARSE_IP_TYPE_IPV6)
};


/**
 * Checks whether the given IP type code is valid.
 *
 * @param type      IP type code (PARSE_IP_TYPE)
 *
 * @return  true if valid, else false
 * */
bool parse_ip_check_type_valid ( const int type );


#if 0
/* hex parsing is not implemented yet. */
int parse_ip_hexchr_to_int ( const char c );
#endif


/**
 * Splits the "/<prefixlen>" suffix from a string if it exists.
 *
 * @param s               input string, will be modified inplace
 * @param slen            length of the input string, excluding the terminating null char
 * @param prefixstr_out   the pointer to the beginning of the prefixlen str
 *                        gets stored to this var.
 *                        It may be NULL if no prefixlen str exists,
 *                        which is not necessarily an error.
 *
 * @return pointer to begin of address string, NULL on error
 * */
char* parse_ip_split_prefixlen (
    char* const restrict s,
    const size_t slen,
    const char** const restrict prefixstr_out
);

/**
 * Converts a str representing a byte (8 bits) into an integer
 * and checks its value (0..255).
 *
 * The resulting byte is stored in the bout variable.
 *
 * @return 0 on success, else non-zero
 * */
int parse_ip_read_octet (
    const char* const restrict arg,
    const int base,
    uint_fast8_t* const restrict bout
);


/** 16-bit variant */
int parse_ip_read_double_octet (
    const char* const restrict arg,
    const int base,
    uint_fast16_t* const restrict bout
);

#endif  /* _HAVE_UTIL_PARSE_IP_COMMON_H_ */
