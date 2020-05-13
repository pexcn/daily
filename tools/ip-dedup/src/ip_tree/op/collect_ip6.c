#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#include "collect_ip6.h"

#include "../data/node.h"
#include "../data/typedesc.h"
#include "../data/tree.h"

#include "../../ip.h"
#include "../../util/dynarray.h"

static int _ip6_tree_collect_addr_into (
   const struct ip_tree_typedesc* const restrict tdesc,
   struct dynarray* const restrict darr,
   struct ip_tree_node* const restrict node
);


int ip6_tree_collect_addr_into (
   struct ip_tree* const restrict tree,
   struct dynarray* const restrict darr
) {
   if ( (tree == NULL) || (darr == NULL) ) {
      errno = EINVAL;
      return -1;

   } else {
      return _ip6_tree_collect_addr_into (
         tree->tdesc, darr, tree->root
      );
   }
}


struct dynarray* ip6_tree_collect_addr ( struct ip_tree* const restrict tree ) {
   struct dynarray* darr;

   darr = new_dynarray ( 1 );
   if ( darr != NULL ) {
      dynarray_set_data_readonly ( darr );

      if ( ip6_tree_collect_addr_into ( tree, darr ) != 0 ) {
         dynarray_free ( darr );
         darr = NULL;
      }
   }

   return darr;
}


static int _ip6_tree_collect_addr_into (
   const struct ip_tree_typedesc* const restrict tdesc,
   struct dynarray* const restrict darr,
   struct ip_tree_node* const restrict node
) {
   if ( node == NULL ) {
      return 0;

   } else if ( node->hot ) {
      if ( dynarray_append ( darr, (void*) (&((node->addr).v4)) ) != 0 ) {
         return -1;
      } else {
         /* no recursion on hot node */
         return 0;
      }

   } else if ( _ip6_tree_collect_addr_into ( tdesc, darr, node->left ) != 0 ) {
      return -1;

   } else if ( _ip6_tree_collect_addr_into ( tdesc, darr, node->right ) != 0 ) {
      return -1;

   } else {
      return 0;
   }
}
