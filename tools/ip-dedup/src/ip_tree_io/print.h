#ifndef _HAVE_IP_TREE_IO_PRINT_H_
#define _HAVE_IP_TREE_IO_PRINT_H_

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "../ip_tree.h"

/** function that prints an ip tree to the given output stream.
 *
 * @param outstream   output stream (must not be NULL)
 * @param tree        IP tree (should not be NULL)
 *
 * @return None (implicit)
 * */
typedef void (*fprint_ip_tree) (
    FILE* const restrict,
    struct ip_tree* const restrict
);


/**
 * Prints an IPv4 tree to the given output stream.
 *
 * Output format: one network per line,
 *                <dotted decimal notation> "/" <netmask length>
 * */
void fprint_ip4_tree (
    FILE* const restrict stream,
    struct ip_tree* const restrict tree
);


/**
 * Prints an IPv6 tree to the given output stream.
 *
 * Output format: one network per line,
 *                <colon hex noation> "/" <prefixlen>
 * */
void fprint_ip6_tree (
    FILE* const restrict stream,
    struct ip_tree* const restrict tree
);


/**
 * Prints an IPv6 tree in compact form to the given output stream.
 *
 * Output format: one network per line,
 *                <compact hex notation> "/" <prefixlen>
 * Examples:
 * - ::/128
 * - fd00:1234:abcd::/48
 * - fe80::/10
 * */
int fprint_ip6_tree_compact (
    FILE* const restrict stream,
    struct ip_tree* const restrict tree
);

#endif
