#ifndef _HAVE_IP_TREE_OP_COLLECT_IP6_H_
#define _HAVE_IP_TREE_OP_COLLECT_IP6_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "../data/tree.h"

#include "../../ip.h"
#include "../../util/dynarray.h"


/**
 * Appends shared pointers to the addresses
 * of all hot nodes in the tree to the given dynarray.
 *
 * Note: top-down DFS, starts at prefixlen 0 and prefers the left path (bit==0),
 * resulting in sorted output (ascending order).
 *
 * @param tree  IP tree (should not be NULL)
 * @param darr  dynarray in which to store IPv6 addresses
 *
 * @return  non-zero on error
 * */
int ip6_tree_collect_addr_into (
   struct ip_tree* const restrict tree,
   struct dynarray* const restrict darr
);


/**
 * Similar to ip6_tree_collect_addr_into(),
 * but creates and returns a new dynarray.
 *
 * @param tree  IP tree (should not be NULL)
 *
 * @return  dynarray with IPv6 address, NULL on error
 * */
struct dynarray* ip6_tree_collect_addr ( struct ip_tree* const restrict tree );

#endif  /* _HAVE_IP_TREE_OP_COLLECT_IP6_H_ */
