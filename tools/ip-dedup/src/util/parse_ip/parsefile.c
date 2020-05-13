#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "parsefile.h"

#include "common.h"
#include "combined.h"

#include "../readfile.h"



static void parse_ip_file_config_init_null (
    struct parse_ip_file_config* const cfg
);



static int parse_ip_file_init_parse_mode (
    struct parse_ip_file_state* const pfile_state,
    const int parse_mode
);


static int _parse_ip_file_next (
    struct parse_ip_file_state* const pfile_state
);


static void parse_ip_file_config_init_null (
    struct parse_ip_file_config* const cfg
) {
    cfg->keep_going     = false;
    cfg->strict_netaddr = false;
}

static int parse_ip_file_init_parse_mode (
    struct parse_ip_file_state* const pfile_state,
    const int parse_mode
) {
    switch ( parse_mode ) {
        case PARSE_IP_TYPE_IPV4:
            pfile_state->f_parse_addr = parse_ip4_addr_combined;
            pfile_state->f_parse_net_addr = parse_ip4_net_addr_combined;
            return 0;

        case PARSE_IP_TYPE_IPV6:
            pfile_state->f_parse_addr = parse_ip6_addr_combined;
            pfile_state->f_parse_net_addr = parse_ip6_net_addr_combined;
            return 0;

        case PARSE_IP_TYPE_BOTH:
            pfile_state->f_parse_addr = parse_ip_addr_combined;
            pfile_state->f_parse_net_addr = parse_ip_net_addr_combined;
            return 0;

        default:
            return -1;
    }
}


void parse_ip_file_state_init_null (
    struct parse_ip_file_state* const pfile_state
) {
    parse_ip_file_config_init_null ( &(pfile_state->cfg) );
    readfile_state_init_null ( &(pfile_state->_file) );
    parse_ip_addr_data_init_null ( &(pfile_state->addr) );

    pfile_state->f_parse_addr       = NULL;
    pfile_state->f_parse_net_addr   = NULL;

    pfile_state->read_ret           = 0;
    pfile_state->keep_going_status  = PARSE_IP_KEEP_GOING_SEEN_NONE;
}


void parse_ip_file_state_free_data (
    struct parse_ip_file_state* const pfile_state
) {
    if ( pfile_state == NULL ) { return; }

    readfile_state_free_data ( &(pfile_state->_file) );
    parse_ip_addr_data_init_free_data ( &(pfile_state->addr) );
}


int parse_ip_file_init_stream (
    struct parse_ip_file_state* const pfile_state,
    FILE* const input_stream,
    const int parse_mode
) {
    parse_ip_file_state_init_null ( pfile_state );

    if ( parse_ip_file_init_parse_mode ( pfile_state, parse_mode ) != 0 ) {
        return -1;
    }

    /* reinit readfile state, no free() on error necessary */
    return readfile_init_stream ( &(pfile_state->_file), input_stream );
}


static int _parse_ip_file_next (
    struct parse_ip_file_state* const pfile_state
) {
    pfile_state->read_ret  = readfile_next_effective ( &(pfile_state->_file) );
    (pfile_state->addr).addr_type = PARSE_IP_TYPE_NONE;

    if ( (pfile_state->read_ret) == READFILE_RET_LINE ) {
        parse_ip_addr_func const f_parse = (
            (pfile_state->cfg).strict_netaddr ? pfile_state->f_parse_net_addr
            : pfile_state->f_parse_addr
        );

        return f_parse (
            (pfile_state->_file).line,
            (pfile_state->_file).line_len,
            &(pfile_state->addr)
        );

    } else if ( (pfile_state->read_ret) == READFILE_RET_EOF ) {
        return PARSE_IP_RET_EOF;

    } else {
        return PARSE_IP_RET_READ_ERR;
    }
}


int parse_ip_file_next (
    struct parse_ip_file_state* const pfile_state
) {
    int parse_ret;

    do {
        parse_ret = _parse_ip_file_next ( pfile_state );

        switch ( parse_ret ) {
            case PARSE_IP_RET_SUCCESS:
                /* no need to check whether at least one addr is set */
                pfile_state->keep_going_status |= PARSE_IP_KEEP_GOING_SEEN_ADDR;
                return parse_ret;

            case PARSE_IP_RET_INVALID:
            case PARSE_IP_RET_INVALID_NET:
                pfile_state->keep_going_status |= PARSE_IP_KEEP_GOING_SEEN_INVALID;

                if ( ( pfile_state->cfg).keep_going ) {
                    /* next token, please */
                    break;
                } else {
                    return parse_ret;
                }

            default:
                /* EOF, FAIL, ... */
                return parse_ret;
        }
    } while (1);
}


int parse_ip_file_eof_eval_keep_going_status (
    struct parse_ip_file_state* const pfile_state
) {
    const unsigned k = pfile_state->keep_going_status;

    /* empty file is ok. */
    if ( k == PARSE_IP_KEEP_GOING_SEEN_NONE ) {
        return PARSE_IP_RET_SUCCESS;

    /* at least one valid addr is okay as well */
    } else if ( (k & PARSE_IP_KEEP_GOING_SEEN_ADDR) != 0 ) {
        return PARSE_IP_RET_SUCCESS;

    /* however, invalid input only is not */
    } else if ( (k & PARSE_IP_KEEP_GOING_SEEN_INVALID) != 0 ) {
        return PARSE_IP_RET_INVALID;

    /* anything else implies broken software logic */
    } else {
        return PARSE_IP_RET_FAIL;
    }
}
