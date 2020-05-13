#ifndef _HAVE_UTIL_PARSE_IP_COMBINED_H_
#define _HAVE_UTIL_PARSE_IP_COMBINED_H_

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "common.h"
#include "ip4.h"
#include "ip6.h"
#include "mixed.h"

#include "../../ip.h"


struct parse_ip_addr_data {
    int addr_type;
    struct ip4_addr_t addr_v4;
    struct ip6_addr_t addr_v6;
};


void parse_ip_addr_data_init_null (
    struct parse_ip_addr_data* const pstate
);

void parse_ip_addr_data_init_free_data (
    struct parse_ip_addr_data* const pstate
);


/**
 * parse_ip_addr_func()
 * prototype for parse_ip functions that convert a single input string
 * into a IPv4 or IPv6 address - or both, depending on which actual
 * implementation this prototype has been called.
 *
 * These functions operate on a shared buffer.
 * The buffer will be overwritten as needed, meaning that it may contain
 * a previous IPv6 address after parsing an IPv4 address,
 * so check the return code and the buffer's addr_type first.
 *
 * @param line      input string
 * @param slen      length of the input string excluding terminating null char
 * @param pstate    shared buffer
 *
 * @return PARSE_IP_RET status codes, non-zero indicates an error
 * */
typedef int (*parse_ip_addr_func) (
    char* const restrict,
    const size_t,
    struct parse_ip_addr_data* const restrict
);


int parse_ip4_addr_combined (
    char* const restrict line,
    const size_t slen,
    struct parse_ip_addr_data* const restrict pstate
);


int parse_ip6_addr_combined (
    char* const restrict line,
    const size_t slen,
    struct parse_ip_addr_data* const restrict pstate
);


int parse_ip_addr_combined (
    char* const restrict line,
    const size_t slen,
    struct parse_ip_addr_data* const restrict pstate
);


/* Variants that check that no host bits are set in the parsed addr. */
int parse_ip4_net_addr_combined (
    char* const restrict line,
    const size_t slen,
    struct parse_ip_addr_data* const restrict pstate
);


int parse_ip6_net_addr_combined (
    char* const restrict line,
    const size_t slen,
    struct parse_ip_addr_data* const restrict pstate
);


int parse_ip_net_addr_combined (
    char* const restrict line,
    const size_t slen,
    struct parse_ip_addr_data* const restrict pstate
);


#endif  /* _HAVE_UTIL_PARSE_IP_COMBINED_H_ */
