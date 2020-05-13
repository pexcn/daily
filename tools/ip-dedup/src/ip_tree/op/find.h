#ifndef _HAVE_IP_TREE_OP_FIND_H_
#define _HAVE_IP_TREE_OP_FIND_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "../data/node.h"
#include "../data/typedesc.h"
#include "../data/tree.h"
#include "../../ip.h"

enum {
    IP_TREE_FIND_HIT = 0,
    IP_TREE_FIND_HIT_HOT_PARENT,
    IP_TREE_FIND_MISS
};


/**
 * Searches for a specific address in the IP tree.
 *
 * Note: iterative
 *
 * @param tree:         collapsed tree (should not be NULL)
 * @param var_addr:     address to locate in the tree
 * @param parent_out:   outvar for storing a pointer to the node's parent
 *                      will contain closest parent that find() could locate,
 *                      even if node could not be found
 *                      will be NULL if searched addr has prefixlen 0
 *
 * @param node_out:     outvar for storing a pointer to the node
 *                      will be NULL if node not found
 *
 * @return IP_TREE_FIND status codes (0 if found), -1 on severe errors
 * */
int ip_tree_find (
   struct ip_tree* const restrict tree,
   const ip_addr_variant_t* const restrict var_addr,
   struct ip_tree_node** const restrict parent_out,
   struct ip_tree_node** const restrict node_out
);


/**
 * tree-less ip_tree_find() that starts at the given node. *
 * The type descriptor has to be passed separately.
 *
 * Note: iterative
 *
 * This is a helper method for tree operations
 * that need to inspect subtrees.
 * External users should call ip_tree_find().
 *
 * @param tdesc:            type descriptor, node operations
 * @param root:             start at this node
 * @param var_addr:
 * @param parent_out:
 * @param node_out:
 *
 * @return IP_TREE_FIND status codes (0 if found), -1 on severe errors
 * */
int ip_tree_find_node (
   const struct ip_tree_typedesc* const tdesc,
   struct ip_tree_node* const root,
   const ip_addr_variant_t* const restrict var_addr,
   struct ip_tree_node** const restrict parent_out,
   struct ip_tree_node** const restrict node_out
);

#endif  /* _HAVE_IP_TREE_OP_FIND_H_ */
