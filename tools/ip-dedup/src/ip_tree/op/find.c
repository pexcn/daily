#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "find.h"
#include "../data/node.h"
#include "../data/typedesc.h"
#include "../data/tree.h"
#include "../../ip.h"


int ip_tree_find (
   struct ip_tree* const restrict tree,
   const ip_addr_variant_t* const restrict var_addr,
   struct ip_tree_node** const restrict parent_out,
   struct ip_tree_node** const restrict node_out
) {
    return ip_tree_find_node (
        tree->tdesc,
        tree->root,
        var_addr,
        parent_out,
        node_out
    );
}


int ip_tree_find_node (
   const struct ip_tree_typedesc* const tdesc,
   struct ip_tree_node* const root,
   const ip_addr_variant_t* const restrict var_addr,
   struct ip_tree_node** const restrict parent_out,
   struct ip_tree_node** const restrict node_out
) {
    /* determine prefixlen == tree depth of var_addr */
    const ip_prefixlen_t prefixlen = tdesc->f_get_addr_prefixlen ( var_addr );

    const ip_prefixlen_t min_prefixpos = (
        tdesc->f_get_addr_prefixlen(&(root->addr))
    );

   /*
    * index 0..(MAX_PREFIXLEN - 1) that indicates how many bits are set
    * in the netmask of the current node.
    * This is equivalent to the depth in the data structure
    * if the tree's root starts at prefixlen 0.
    * */
    ip_prefixlen_t cur_prefixpos;

    /* current node in the subnet path - shared, do not free */
    struct ip_tree_node* cur_node;

    /* pointer to the next node in the subnet path, child node of cur_node */
    /* shared, do not free */
    struct ip_tree_node* sub_node;

    *parent_out = NULL;
    *node_out   = NULL;

    /* root node should exist */
    if ( root == NULL ) { return -1; }

    /* prefixlen should be within valid range (always >= 0 due to uint) */
    if (
        (prefixlen < min_prefixpos)
        || (prefixlen > (tdesc->max_prefixlen))
    ) {
        /* will not find addr */
        return -1;
    }

    /*
     * search for addr
     *
     * start at level min_prefixpos (usually 0),
     * cur_node points to the parent node,
     * sub_node to the next not-NULL node
     *
     * sub_node points to the target node after this loop
     * */
    cur_node = NULL;
    sub_node = root;
    for (
        cur_prefixpos = min_prefixpos;
        cur_prefixpos < prefixlen;
        cur_prefixpos++
    ) {
        const ip_prefixlen_t sub_prefixpos = ( cur_prefixpos + 1 );

        cur_node = sub_node;
        /*sub_node = NULL;  // redundant */

        if ( cur_node->hot ) {
            /*
             * addr to be found overshadowed by hot node (at upper level)
             */
            *parent_out = cur_node;
            *node_out   = NULL;     /* redundant */

            return IP_TREE_FIND_HIT_HOT_PARENT;

        } else if (
            ! ( tdesc->f_check_addr_bit_set) ( var_addr, sub_prefixpos )
        ) {
            /* bit is unset -> follow left path */
            sub_node = cur_node->left;

        } else {
            /* bit is set -> follow right path */
            sub_node = cur_node->right;
        }

        /* if the requested child node does not exist, then neither does var_addr */
        if ( sub_node == NULL ) {
            *parent_out = cur_node;
            *node_out   = NULL;     /* redundant */

            return IP_TREE_FIND_MISS;
        } /* else continue with next node */
    }

    *parent_out = cur_node;     /* which may be NULL if prefixlen==0 */
    *node_out   = sub_node;     /* which is not NULL */
    return IP_TREE_FIND_HIT;
}
