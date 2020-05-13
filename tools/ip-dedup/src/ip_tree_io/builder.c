#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "builder.h"

#include "../ip_tree.h"
#include "../ip.h"

#include "../util/readfile.h"
#include "../util/dynarray.h"
#include "../util/parse_ip.h"


static struct ip_tree_build_data* ip_tree_build_data_new_empty (void);
static int ip_tree_build_data_init_v4 ( struct ip_tree_build_data* const obj );
static int ip_tree_build_data_init_v6 ( struct ip_tree_build_data* const obj );

static int ip_tree_builder_process_parsed__insert (
    struct ip_tree_build_data* const restrict obj,
    struct parse_ip_addr_data* const restrict vaddr
);


static struct ip_tree_build_data* ip_tree_build_data_new_empty (void) {
    struct ip_tree_build_data* obj;

    obj = malloc ( sizeof *obj );
    if ( obj != NULL ) {
        obj->v4         = NULL;
        obj->v6         = NULL;
        obj->tree_mode  = PARSE_IP_TYPE_NONE;

        obj->did_read_stdin = false;
    }

    return obj;
}


static int ip_tree_build_data_init_v4 ( struct ip_tree_build_data* const obj ) {
    obj->v4 = ip4_tree_new();
    return ( obj->v4 != NULL ) ? 0 : -1;
}


static int ip_tree_build_data_init_v6 ( struct ip_tree_build_data* const obj ) {
    obj->v6 = ip6_tree_new();
    return ( obj->v6 != NULL ) ? 0 : -1;
}


struct ip_tree_build_data* ip_tree_builder_new ( const int tree_mode ) {
    struct ip_tree_build_data* obj;

    if ( ! parse_ip_check_type_valid ( tree_mode ) ) { return NULL; }

    obj = ip_tree_build_data_new_empty();
    if ( obj != NULL ) {
        obj->tree_mode = tree_mode;

        if ( (obj->tree_mode & PARSE_IP_TYPE_IPV4) != 0 ) {
            if ( ip_tree_build_data_init_v4 ( obj ) != 0 ) {
                ip_tree_builder_destroy ( &obj );
                return NULL;
            }
        }

        if ( (obj->tree_mode & PARSE_IP_TYPE_IPV6) != 0 ) {
            if ( ip_tree_build_data_init_v6 ( obj ) != 0 ) {
                ip_tree_builder_destroy ( &obj );
                return NULL;    /* redundant */
            }
        }
    }

    return obj;
}


void ip_tree_builder_destroy ( struct ip_tree_build_data** const obj_ptr ) {
    if ( *obj_ptr != NULL ) {
        ip_tree_destroy ( &((*obj_ptr)->v4) );
        ip_tree_destroy ( &((*obj_ptr)->v6) );

        free ( *obj_ptr );
        *obj_ptr = NULL;
    }
}

struct ip_tree* ip_tree_builder_steal_v4 (
    struct ip_tree_build_data* const obj
) {
    struct ip_tree* tree;

    tree = obj->v4;
    obj->v4 = NULL;
    return tree;
}


struct ip_tree* ip_tree_builder_steal_v6 (
    struct ip_tree_build_data* const obj
) {
    struct ip_tree* tree;

    tree = obj->v6;
    obj->v6 = NULL;
    return tree;
}


static int ip_tree_builder_process_parsed__insert (
    struct ip_tree_build_data* const restrict obj,
    struct parse_ip_addr_data* const restrict vaddr
) {
    int ret;

    if ( (vaddr->addr_type & PARSE_IP_TYPE_IPV4) != 0 ) {
        ret = ip4_tree_insert ( obj->v4, &(vaddr->addr_v4) );
        if ( ret != 0 ) { return -1; }
    }

    if ( (vaddr->addr_type & PARSE_IP_TYPE_IPV6) != 0 ) {
        ret = ip6_tree_insert ( obj->v6, &(vaddr->addr_v6) );
        if ( ret != 0 ) { return -1; }
    }

    return 0;
}


int ip_tree_builder_parse_stream_do (
    ip_tree_build_process_parsed_func f_process_parsed,
    struct ip_tree_build_data* const restrict obj,
    FILE* const restrict input_stream,
    const bool keep_going,
    const bool strict_netaddr
) {
    struct parse_ip_file_state pfile_state;
    int parse_ret;
    int ret;

    if ( (input_stream == stdin) && (obj->did_read_stdin) ) {
        /* will not read stdin twice */
        return PARSE_IP_RET_BAD_INFILE_DUP;
    }

    if (
        parse_ip_file_init_stream (
            &pfile_state, input_stream, obj->tree_mode
        ) != 0
    ) {
        return -1;
    }

    /* init cfg */
    pfile_state.cfg.keep_going     = keep_going;
    pfile_state.cfg.strict_netaddr = strict_netaddr;

    if ( input_stream == stdin ) { obj->did_read_stdin = true; }

    do {
        parse_ret = parse_ip_file_next ( &pfile_state );

        switch ( parse_ret ) {
            case PARSE_IP_RET_SUCCESS:
                // process parsed
                ret = f_process_parsed ( obj, &(pfile_state.addr) );
                if ( ret != PARSE_IP_RET_SUCCESS ) {
                    parse_ip_file_state_free_data ( &pfile_state );
                    return ret;
                } /* else continue with next token */
                break;

            case PARSE_IP_RET_EOF:
                // regular function end here
                ret = parse_ip_file_eof_eval_keep_going_status ( &pfile_state );
                parse_ip_file_state_free_data ( &pfile_state );
                return ret;

            default:
                parse_ip_file_state_free_data ( &pfile_state );
                return parse_ret;
        }
    } while (1);
}


int ip_tree_builder_parse_files_do (
    ip_tree_build_process_parsed_func f_process_parsed,
    struct ip_tree_build_data* const restrict obj,
    struct dynarray* const input_files,
    const bool keep_going,
    const bool strict_netaddr
) {
    size_t k;
    FILE* input_stream;
    int ret;

    if ( input_files == NULL ) { return PARSE_IP_RET_BAD_INFILE; }

    ret = PARSE_IP_RET_SUCCESS;

    for (
        k = 0;
        ( (k < (input_files->len)) && (ret == PARSE_IP_RET_SUCCESS) );
        k++
    ) {
        const char* const input_file = dynarray_get_const_str ( input_files, k );

        if ( (input_file == NULL) || (*input_file == '\0') ) {
            ret = PARSE_IP_RET_BAD_INFILE;

        } else if ( (*input_file == '-') && (*(input_file + 1) == '\0') ) {
            ret = ip_tree_builder_parse_stream_do (
                f_process_parsed, obj, stdin,
                keep_going, strict_netaddr
            );

        } else {
            input_stream = fopen ( input_file, "r" );

            if ( input_stream == NULL ) {
                ret = PARSE_IP_RET_READ_ERR;

            } else {
                ret = ip_tree_builder_parse_stream_do (
                    f_process_parsed, obj, input_stream,
                    keep_going, strict_netaddr
                );

                fclose ( input_stream );  /* retcode ignored */
            }
        }
    }

    return ret;
}



int ip_tree_builder_parse_stream_do_insert (
    struct ip_tree_build_data* const restrict obj,
    FILE* const restrict input_stream,
    const bool keep_going,
    const bool strict_netaddr
) {
    return ip_tree_builder_parse_stream_do (
        ip_tree_builder_process_parsed__insert,
        obj,
        input_stream,
        keep_going,
        strict_netaddr
    );
}


int ip_tree_builder_parse_files_do_insert (
    struct ip_tree_build_data* const restrict obj,
    struct dynarray* const input_files,
    const bool keep_going,
    const bool strict_netaddr
) {
    return ip_tree_builder_parse_files_do (
        ip_tree_builder_process_parsed__insert,
        obj,
        input_files,
        keep_going,
        strict_netaddr
    );
}


int ip_tree_builder_purge (
    struct ip_tree_build_data* const restrict obj,
    struct ip_tree* const restrict purge_tree_v4,
    struct ip_tree* const restrict purge_tree_v6
) {
    size_t k;
    struct dynarray* arr;

    if ( purge_tree_v4 != NULL ) {
        arr = ip4_tree_collect_addr ( purge_tree_v4 );
        if ( arr == NULL ) { return -1; }

        dynarray_foreach ( arr, k ) {
            const ip_addr_variant_t* const addr = dynarray_get_as (
                arr, k, const ip_addr_variant_t*
            );

            if ( ip_tree_purge ( obj->v4, addr ) != 0 ) {
                dynarray_free_ptr ( &arr );
                return -1;
            }
        }

        dynarray_free_ptr ( &arr );
    }

    if ( purge_tree_v6 != NULL ) {
        arr = ip6_tree_collect_addr ( purge_tree_v6 );
        if ( arr == NULL ) { return -1; }

        dynarray_foreach ( arr, k ) {
            const ip_addr_variant_t* const addr = dynarray_get_as (
                arr, k, const ip_addr_variant_t*
            );

            if ( ip_tree_purge ( obj->v6, addr ) != 0 ) {
                dynarray_free_ptr ( &arr );
                return -1;
            }
        }

        dynarray_free_ptr ( &arr );
    }


    return 0;
}
