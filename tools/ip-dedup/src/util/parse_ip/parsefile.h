#ifndef _HAVE_UTIL_PARSE_IP_PARSEFILE_H_
#define _HAVE_UTIL_PARSE_IP_PARSEFILE_H_

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "combined.h"
#include "../readfile.h"


enum {
    PARSE_IP_KEEP_GOING_SEEN_NONE    = 0x0,
    PARSE_IP_KEEP_GOING_SEEN_ADDR    = 0x1,
    PARSE_IP_KEEP_GOING_SEEN_INVALID = 0x2
};



struct parse_ip_file_config {
    bool keep_going;
    bool strict_netaddr;
};


struct parse_ip_file_state {
    struct parse_ip_file_config cfg;

    struct readfile_state     _file;
    struct parse_ip_addr_data addr;

    parse_ip_addr_func f_parse_addr;
    parse_ip_addr_func f_parse_net_addr;

    int read_ret;                   /* most recent readfile return code */
    unsigned keep_going_status;     /* PARSE_IP_KEEP_GOING mask */
};


void parse_ip_file_state_init_null (
    struct parse_ip_file_state* const pfile_state
);


void parse_ip_file_state_free_data (
    struct parse_ip_file_state* const pfile_state
);


int parse_ip_file_init_stream (
    struct parse_ip_file_state* const pfile_state,
    FILE* const input_stream,
    const int parse_mode
);


int parse_ip_file_next (
    struct parse_ip_file_state* const pfile_state
);


int parse_ip_file_eof_eval_keep_going_status (
    struct parse_ip_file_state* const pfile_state
);

#endif  /* _HAVE_UTIL_PARSE_IP_PARSEFILE_H_ */
