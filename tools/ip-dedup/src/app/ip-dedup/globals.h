#ifndef _HAVE_APP_IP_DEDUP_GLOBALS_H_
#define _HAVE_APP_IP_DEDUP_GLOBALS_H_

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "../../ip.h"
#include "../../ip_tree.h"
#include "../../ip_tree_io/builder.h"
#include "../../util/parse_ip/common.h"
#include "../../util/dynarray.h"

/**
 * mode of tree operations, choose from
 * - IPv4 only
 * - IPv6 only
 * - mixed IPv4 / IPv6
 *
 * These are just aliases of PARSE_IP_TYPE.
 * */
enum {
    IPDEDUP_TREE_MODE_NONE  = PARSE_IP_TYPE_NONE,
    IPDEDUP_TREE_MODE_IPV4  = PARSE_IP_TYPE_IPV4,
    IPDEDUP_TREE_MODE_IPV6  = PARSE_IP_TYPE_IPV6,
    IPDEDUP_TREE_MODE_MIXED = PARSE_IP_TYPE_BOTH
};


struct ipdedup_globals;

/**
 * main function dispatcher, usually depends on mode of tree operations
 *
 * ipdedup_main_action :: globals -> int
 * */
typedef int (*ipdedup_main_action) (struct ipdedup_globals* const restrict);

struct ipdedup_globals {
    int tree_mode;
    struct ip_tree_build_data* tree_builder;

    /* shared references to the tree builder's output data, do not free them */
    struct ip_tree* tree_v4;
    struct ip_tree* tree_v6;

    ip_prefixlen_t collapse_prefixlen_v4;
    ip_prefixlen_t collapse_prefixlen_v6;

    const char* datadir;
    size_t datadir_len;

    const char* prog_name;

    const char* outfile;
    FILE* outstream;
    bool close_outstream;

    /* malloc'ed strings that are part of readonly data structures */
    struct dynarray* pile_of_shame;

    struct dynarray* infiles;
    struct dynarray* purge_infiles;

    bool want_invert;
    bool want_keep_going;
    bool want_strict;
    bool want_long_output;
};

/**
 * Initializes the program's global variables.
 *
 * @param g
 *
 * @return 0 on success, non-zero otherwise (e.g. malloc() failed)
 * */
int ipdedup_globals_init ( struct ipdedup_globals* const restrict g );


/**
 * Destroys the program's global variables.
 *
 * @param g
 *
 * @return None (implicit)
 * */
void ipdedup_globals_free ( struct ipdedup_globals* const restrict g );


/**
 * Initializes the tree view attribute.
 *
 * @param g
 *
 * @return None (implicit)
 * */
void ipdedup_globals_init_tree_view ( struct ipdedup_globals* const restrict g );


int ipdedup_globals_pile_of_shame_push (
    struct ipdedup_globals* const restrict g,
    char* const arg
);


void ipdedup_globals_set_datadir (
    struct ipdedup_globals* const restrict g,
    const char* const restrict datadir
);


char* ipdedup_globals_get_datafile_path (
    struct ipdedup_globals* const restrict g,
    const char* const restrict relpath
);

#endif  /* _HAVE_APP_IP_DEDUP_GLOBALS_H_ */
