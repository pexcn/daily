#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>     /* strlen() */
#include <errno.h>
#include <stdio.h>      /* fflush(), fclose(), snprintf */

#include "globals.h"

#include "../../ip.h"
#include "../../ip_tree.h"
#include "../../ip_tree_io/builder.h"
#include "../../util/parse_ip/common.h"
#include "../../util/dynarray.h"

static char* _ipdedup_globals_get_datafile_path (
    const char* const restrict datadir,
    const size_t datadir_len,
    const char* const restrict relpath
);


int ipdedup_globals_init ( struct ipdedup_globals* const restrict g ) {
    g->tree_mode        = IPDEDUP_TREE_MODE_NONE;
    g->tree_builder     = NULL;

    g->tree_v4          = NULL;
    g->tree_v6          = NULL;

    g->collapse_prefixlen_v4 = 0;
    g->collapse_prefixlen_v6 = 0;

    g->datadir          = NULL;
    g->datadir_len      = 0;

    g->prog_name        = NULL;

    g->outfile          = NULL;
    g->outstream        = NULL;
    g->close_outstream  = false;

    g->pile_of_shame    = NULL;

    g->infiles          = NULL;
    g->purge_infiles    = NULL;

    g->want_invert      = false;
    g->want_keep_going  = false;
    g->want_strict      = false;
    g->want_long_output = false;

#ifdef IPDEDUP_DATADIR
    ipdedup_globals_set_datadir ( g, IPDEDUP_DATADIR );
#endif

    return 0;
}


void ipdedup_globals_free ( struct ipdedup_globals* const restrict g ) {
    ip_tree_builder_destroy ( &(g->tree_builder) );

    if ( g->outstream != NULL ) {
        fflush ( g->outstream );        /* ignore retcode */

        if ( g->close_outstream ) {
            fclose ( g->outstream );    /* ignore retcode */
            g->outstream = NULL;
        }
    }

    dynarray_free_ptr ( &(g->pile_of_shame) );

    dynarray_free_ptr ( &(g->infiles) );
    dynarray_free_ptr ( &(g->purge_infiles) );
}


void ipdedup_globals_init_tree_view (
    struct ipdedup_globals* const restrict g
) {
    g->tree_v4 = NULL;
    g->tree_v6 = NULL;

    if ( (g->tree_mode & IPDEDUP_TREE_MODE_IPV4) != 0 ) {
        g->tree_v4 = g->tree_builder->v4;
    }

    if ( (g->tree_mode & IPDEDUP_TREE_MODE_IPV6) != 0 ) {
        g->tree_v6 = g->tree_builder->v6;
    }
}


void ipdedup_globals_set_datadir (
    struct ipdedup_globals* const restrict g,
    const char* const restrict datadir
) {
    if ( (datadir == NULL) || (*datadir == '\0') ) {
        g->datadir     = NULL;
        g->datadir_len = 0;

    } else {
        g->datadir     = datadir;
        g->datadir_len = strlen ( g->datadir );
    }
}


int ipdedup_globals_pile_of_shame_push (
    struct ipdedup_globals* const restrict g,
    char* const arg
) {
    if ( g->pile_of_shame == NULL ) {
        g->pile_of_shame = new_dynarray ( 12 );
        if ( g->pile_of_shame == NULL ) { return -1; }
    }

    return dynarray_append ( g->pile_of_shame, arg );
}


char* ipdedup_globals_get_datafile_path (
    struct ipdedup_globals* const restrict g,
    const char* const restrict relpath
) {
    if ( relpath == NULL ) {
        errno = EINVAL;
        return NULL;

    } else if ( g->datadir_len < 1 ) {
        errno = ENOENT;
        return NULL;

    } else {
        return _ipdedup_globals_get_datafile_path (
            g->datadir, g->datadir_len, relpath
        );
    }
}


static char* _ipdedup_globals_get_datafile_path (
    const char* const restrict datadir,
    const size_t datadir_len,
    const char* const restrict relpath
) {
    const size_t relpath_len = strlen ( relpath );
    const bool insert_sep = ( datadir[datadir_len - 1] != '/' );

    size_t slen;
    int ret;
    char* sbuf;

    if ( relpath_len < 1 ) {
        errno = EINVAL;
        return NULL;
    }

    slen = (
        datadir_len
        + ( insert_sep ? 1 : 0 )
        + relpath_len
    );

    sbuf = malloc ( (slen + 1) * sizeof *sbuf );  /* sizeof char, wow */
    if ( sbuf == NULL ) { return NULL; }

    ret = snprintf (
        sbuf, (slen + 1),
        ( insert_sep ? "%s/%s" : "%s%s" ),
        datadir,
        relpath
    );

    if ( (ret < 0) || (((size_t) ret) != slen) ) {
        free ( sbuf );
        sbuf = NULL;
    }

    return sbuf;
}
