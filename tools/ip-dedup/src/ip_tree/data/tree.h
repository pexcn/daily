#ifndef _HAVE_IP_TREE_DATA_TREE_H_
#define _HAVE_IP_TREE_DATA_TREE_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "node.h"
#include "typedesc.h"

#include "../../ip.h"


struct ip_tree {
   /* type descriptor */
   const struct ip_tree_typedesc* tdesc;

   struct ip_tree_node* root;

   ip_prefixlen_t auto_collapse_prefixlen;
};


struct ip_tree* ip_tree_new ( const struct ip_tree_typedesc* tdesc );

/**
 * Creates an empty IPv4 tree.
 *
 * @returns ip4 tree or NULL on error
 * */
struct ip_tree* ip4_tree_new (void);


/**
 * Creates an empty IPv6 tree.
 *
 * @returns ip6 tree or NULL on error
 * */
struct ip_tree* ip6_tree_new (void);


/**
 * Recursively frees a IPv4 tree data structure referenced by node_ptr.
 *
 * The tree may already be set to NULL (*node_ptr == NULL),
 * in which case no action is taken.
 *
 * After calling this method,
 * the tree will be deleted and *node_ptr will be set to NULL.
 *
 * Note: bottom-up DFS
 *
 * @returns void (implicit)
 * */
void ip_tree_destroy ( struct ip_tree** const restrict tree_ptr );


void ip_tree_set_auto_collapse (
   struct ip_tree* const restrict tree,
   const ip_prefixlen_t prefixlen
);

#endif  /* _HAVE_IP_TREE_DATA_TREE_H_ */
