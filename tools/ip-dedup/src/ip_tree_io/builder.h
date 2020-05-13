#ifndef _HAVE_IP_TREE_IO_BUILD_DATA_H_
#define _HAVE_IP_TREE_IO_BUILD_DATA_H_

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "../ip_tree.h"
#include "../ip.h"

#include "../util/parse_ip.h"
#include "../util/dynarray.h"


/**
 * Depending on the IP tree builder mode,
 * IPv4, IPv6 or trees of both types may be created.
 *
 * This are stored as pointers to the actual data structure here
 * and can later be "stolen" with ip_tree_builder_steal_v4/v6().
 *
 * The parsing backend depends on the tree mode, so store that one, too.
 * */
struct ip_tree_build_data {
    struct ip_tree* v4;
    struct ip_tree* v6;

    int tree_mode;

    bool did_read_stdin;
};


typedef int (*ip_tree_build_process_parsed_func) (
    struct ip_tree_build_data* const restrict,
    struct parse_ip_addr_data* const restrict
);


/**
 * Creates a new tree builder.
 * */
struct ip_tree_build_data* ip_tree_builder_new ( const int tree_mode );


/**
 * Frees a tree builder including attached IP trees (if not stolen before).
 * */
void ip_tree_builder_destroy ( struct ip_tree_build_data** const obj_ptr );


/**
 * Removes the IPv4 tree from the IP builder and passes it to the caller.
 *
 * Note: Parsing new addresses after stealing trees will result in SEGFAULTs.
 * */
struct ip_tree* ip_tree_builder_steal_v4 (
    struct ip_tree_build_data* const obj
);


/**
 * Removes the IPv6 tree from the IP builder and passes it to the caller.
 *
 * Note: Parsing new addresses after stealing trees will result in SEGFAULTs.
 * */
struct ip_tree* ip_tree_builder_steal_v6 (
    struct ip_tree_build_data* const obj
);

int ip_tree_builder_parse_stream_do (
    ip_tree_build_process_parsed_func f_process_parsed,
    struct ip_tree_build_data* const restrict obj,
    FILE* const restrict input_stream,
    const bool keep_going,
    const bool strict_netaddr
);


int ip_tree_builder_parse_files_do (
    ip_tree_build_process_parsed_func f_process_parsed,
    struct ip_tree_build_data* const restrict obj,
    struct dynarray* const input_files,
    const bool keep_going,
    const bool strict_netaddr
);


/**
 * Reads and parses addresses/networks from the given input stream, one per line.
 *
 * @param obj               ip tree builder data
 * @param input_stream      input stream
 * @param keep_going        whether to tolerate invalid input and just
 *                          continue with the next token on tolerable errors
 * @param strict_netaddr    whether to check that no host bits are set
 *                          in parsed addresses
 *
 * @return PARSE_IP_RET status codes, zero means success
 * */
int ip_tree_builder_parse_stream_do_insert (
    struct ip_tree_build_data* const restrict obj,
    FILE* const restrict input_stream,
    const bool keep_going,
    const bool strict_netaddr
);


int ip_tree_builder_parse_files_do_insert (
    struct ip_tree_build_data* const restrict obj,
    struct dynarray* const input_files,
    const bool keep_going,
    const bool strict_netaddr
);


int ip_tree_builder_purge (
    struct ip_tree_build_data* const restrict obj,
    struct ip_tree* const restrict purge_tree_v4,
    struct ip_tree* const restrict purge_tree_v6
);

#endif  /* _HAVE_IP_TREE_IO_BUILD_DATA_H_ */
