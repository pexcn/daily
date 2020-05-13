#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#include "insert.h"
#include "find.h"

#include "../data/node.h"
#include "../data/typedesc.h"
#include "../data/tree.h"

#include "../../ip.h"

/* actual insert function() - no NULL check guards */
static int _ip_tree_insert (
   struct ip_tree* const restrict tree,
   const ip_addr_variant_t* const restrict var_addr
);

static int _ip_tree_insert_walkdown_create (
   struct ip_tree* const restrict tree,
   const ip_addr_variant_t* const restrict var_addr,
   struct ip_tree_node* const restrict parent
);


int ip_tree_insert (
   struct ip_tree* const restrict tree,
   const ip_addr_variant_t* const restrict var_addr
) {
   if ( (tree == NULL) || (var_addr == NULL) ) {
      errno = EINVAL;
      return -1;
   } else {
      return _ip_tree_insert ( tree, var_addr );
   }
}


int ip4_tree_insert (
   struct ip_tree* const restrict tree,
   const struct ip4_addr_t* const restrict addr
) {
   ip_addr_variant_t var_addr;

   if ( addr == NULL ) {
      errno = EINVAL;
      return -1;

   } else {
      var_addr.v4.addr      = addr->addr;
      var_addr.v4.prefixlen = addr->prefixlen;

      return ip_tree_insert ( tree, &var_addr );
   }
}


int ip6_tree_insert (
   struct ip_tree* const restrict tree,
   const struct ip6_addr_t* const restrict addr
) {
   ip_addr_variant_t var_addr;

   if ( addr == NULL ) {
      errno = EINVAL;
      return -1;

   } else {
      var_addr.v6.addr      = addr->addr;
      var_addr.v6.prefixlen = addr->prefixlen;

      return ip_tree_insert ( tree, &var_addr );
   }
}


static int _ip_tree_insert (
   struct ip_tree* const restrict tree,
   const ip_addr_variant_t* const restrict var_addr
) {
   int find_ret;
   struct ip_tree_node* parent;
   struct ip_tree_node* node;

   /*
    * find anchor
    * - if addr already in the tree: mark as hot
    * - otherwise: start at depth-most existing parent and create nodes downto addr
    *
    * Note that even if auto-collapse is enabled,
    * ip_tree_find() might return nodes with prefixlen > auto-collapse
    * if nodes have been inserted prior to enabling this feature.
    * */
   find_ret = ip_tree_find ( tree, var_addr, &parent, &node );

   switch ( find_ret ) {
      case IP_TREE_FIND_HIT_HOT_PARENT:
         /* addr to be inserted overshadowed by hot (grand)parent */
         return 0;

      case IP_TREE_FIND_HIT:
         if ( ! (node->hot) ) {
            /* mark node as hot */
            node->hot = true;

            /* purge intermediate child nodes (if they exist) */
            tree->tdesc->f_destroy ( &(node->left) );
            tree->tdesc->f_destroy ( &(node->right) );
         }
         return 0;

      case IP_TREE_FIND_MISS:
         return _ip_tree_insert_walkdown_create ( tree, var_addr, parent );

      default:
         return -1;
   }
}


static int _ip_tree_insert_walkdown_create (
   struct ip_tree* const restrict tree,
   const ip_addr_variant_t* const restrict var_addr,
   struct ip_tree_node* const restrict parent
) {
   ip_prefixlen_t prefixlen;
   ip_prefixlen_t cur_prefixpos;

   struct ip_tree_node* cur_node;
   struct ip_tree_node* sub_node;

   /* determine effective prefixlen, subject to auto-collapse */
   prefixlen = (
      tree->tdesc->f_get_addr_prefixlen ( var_addr )
   );

   if (
      ( tree->auto_collapse_prefixlen > 0 )
      && ( prefixlen > tree->auto_collapse_prefixlen )
   ) {
      prefixlen = tree->auto_collapse_prefixlen;
   }

   cur_node = NULL;
   sub_node = parent;
   for (
      cur_prefixpos = tree->tdesc->f_get_addr_prefixlen(&(sub_node->addr));
      cur_prefixpos < prefixlen;
      cur_prefixpos++
   ) {
      const ip_prefixlen_t sub_prefixpos = ( cur_prefixpos + 1 );

      cur_node = sub_node;

      if ( ! ( tree->tdesc->f_check_addr_bit_set) ( var_addr, sub_prefixpos ) ) {
         /* bit is unset -> create left */
         sub_node = tree->tdesc->f_new_child ( cur_node, false, false );
         if ( sub_node == NULL ) { return -1; }
         cur_node->left = sub_node;

      } else {
         /* bit is set -> create right */
         sub_node = tree->tdesc->f_new_child ( cur_node, true, false );
         if ( sub_node == NULL ) { return -1; }
         cur_node->right = sub_node;
      }
   }

   /* mark most recent node as hot */
   sub_node->hot = true;

   return 0;
}
