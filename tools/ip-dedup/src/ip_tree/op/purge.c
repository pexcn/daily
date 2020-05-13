#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#include "purge.h"
#include "find.h"
#include "insert.h"
#include "invert.h"

#include "../data/node.h"
#include "../data/typedesc.h"
#include "../data/tree.h"

#include "../../ip.h"


/* actual purge function - no NULL check guards */
static int _ip_tree_purge (
   struct ip_tree* const restrict tree,
   const ip_addr_variant_t* const restrict var_addr
);


static int _ip_tree_purge_replace_node (
   struct ip_tree* const restrict tree,
   struct ip_tree_node* const parent,
   struct ip_tree_node* const node
);


static int _ip_tree_purge_replace_parent (
   struct ip_tree* const restrict tree,
   const ip_addr_variant_t* const restrict var_addr,
   struct ip_tree_node* const parent
);


int ip_tree_purge (
    struct ip_tree* const restrict tree,
    const ip_addr_variant_t* const restrict var_addr
) {
    if ( (tree == NULL) || (var_addr == NULL) ) {
        errno = EINVAL;
        return -1;

    } else {
        return _ip_tree_purge ( tree, var_addr );
    }
}


/*
 * How to purge:
 *
 * status <- find parent, node
 *
 * if HIT:
 *    if prefixlen == 0:   (more generally, prefixlen > prefixlen of tree root)
 *       remove "hot" marker from node
 *       destroy node's l/r children
 *    else:
 *       detach node from parent
 *       destroy node recursively
 *
 * elif HOT_HOT_PARENT:
 *    create temporary tree TTREE with root @ parent prefixlen
 *    insert addr to be purged into TTREE  // collapse not needed
 *    invert TTREE
 *    remove "hot" marker from parent
 *    move l/r children of TTREE to parent
 *    destroy TTREE
 *
 * elif MISS:
 *    no-op
 *
 * else:
 *    error
 * */
static int _ip_tree_purge (
   struct ip_tree* const restrict tree,
   const ip_addr_variant_t* const restrict var_addr
) {
    int find_ret;
    struct ip_tree_node* parent;
    struct ip_tree_node* node;

    find_ret = ip_tree_find ( tree, var_addr, &parent, &node );

    switch ( find_ret ) {
        case IP_TREE_FIND_HIT_HOT_PARENT:
            return _ip_tree_purge_replace_parent ( tree, var_addr, parent );

        case IP_TREE_FIND_HIT:
            return _ip_tree_purge_replace_node ( tree, parent, node );

        case IP_TREE_FIND_MISS:
            return 0;

        default:
            return -1;
    }
}


static int _ip_tree_purge_replace_node (
   struct ip_tree* const restrict tree,
   struct ip_tree_node* const parent,
   struct ip_tree_node* const node
) {
    if ( node == tree->root ) {
        /* handle root */
        node->hot = false;
        tree->tdesc->f_destroy ( &(node->left) );
        tree->tdesc->f_destroy ( &(node->right) );

    } else {
        /* detach from parent */
        if ( parent->left == node ) {
            tree->tdesc->f_destroy ( &(parent->left) );

        } else if ( parent->right == node ) {
            tree->tdesc->f_destroy ( &(parent->right) );

        } else {
            /* assertion error */
            return -1;
        }
    }

    return 0;
}


static int _ip_tree_purge_replace_parent (
   struct ip_tree* const restrict tree,
   const ip_addr_variant_t* const restrict var_addr,
   struct ip_tree_node* const parent
) {
    int ret;
    struct ip_tree* repl_tree;

    repl_tree = ip_tree_new ( tree->tdesc );
    if ( repl_tree == NULL ) { return -1; }

    /* start new subtree at parent */
    repl_tree->root->addr = parent->addr;   /* struct copy */

    if ( ip_tree_insert ( repl_tree, var_addr ) != 0 ) {
        /* stand by for cleanup */
        ret = -1;

    } else if ( ip_tree_invert ( repl_tree ) != 0 ) {
        /* stand by for cleanup */
        ret = -1;

    } else {
        parent->hot = false;

        /* transfer l/r nodes from repl_tree to parent */
        tree->tdesc->f_destroy ( &(parent->left) );
        tree->tdesc->f_destroy ( &(parent->right) );

        parent->left  = repl_tree->root->left;
        parent->right = repl_tree->root->right;

        repl_tree->root->left  = NULL;
        repl_tree->root->right = NULL;

        ret = 0;
    }

    ip_tree_destroy ( &repl_tree );
    return ret;
}
