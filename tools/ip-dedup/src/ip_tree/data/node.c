#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "node.h"
#include "../../ip.h"

/** Common node null initialization code. */
static void _ip_tree_node_init_null ( struct ip_tree_node* const restrict node );

static void _ip_tree_node_init_null ( struct ip_tree_node* const restrict node ) {
    node->left  = NULL;
    node->right = NULL;

    node->hot   = false;
    /* node->addr remains unmodified - memset(...) */
}


bool ip_tree_node_left_hot ( const struct ip_tree_node* const restrict node ) {
    return ( (node->left == NULL) ? false : (node->left->hot) );
}

bool ip_tree_node_right_hot ( const struct ip_tree_node* const restrict node ) {
    return ( (node->right == NULL) ? false : (node->right->hot) );
}


/* IPv4 / IPv6 functions */
/* init_null() */
void ip4_tree_node_init_null ( struct ip_tree_node* const restrict node ) {
    _ip_tree_node_init_null ( node );
    ip4_addr_init_null ( &((node->addr).v4) );
}

void ip6_tree_node_init_null ( struct ip_tree_node* const restrict node ) {
   _ip_tree_node_init_null ( node );
   ip6_addr_init_null ( &((node->addr).v6) );
}

/* new_null() */
struct ip_tree_node* ip4_tree_node_new_null (void) {
    struct ip_tree_node* node;

    node = (struct ip_tree_node*) malloc ( sizeof *node );
    if ( node != NULL ) {
        ip4_tree_node_init_null ( node );
    }

    return node;
}

struct ip_tree_node* ip6_tree_node_new_null (void) {
    struct ip_tree_node* node;

    node = (struct ip_tree_node*) malloc ( sizeof *node );
    if ( node != NULL ) {
        ip6_tree_node_init_null ( node );
    }

    return node;
}

/* new_sibling() */
struct ip_tree_node* ip4_tree_node_new_sibling (
   const struct ip_tree_node* const restrict src,
   const bool hot
) {
   struct ip_tree_node* node;

   node = ip4_tree_node_new_null();
   if ( node != NULL ) {
      node->hot = hot;
      ip4_addr_init_sibling ( &((node->addr).v4), &((src->addr).v4) );
   }

   return node;
}

struct ip_tree_node* ip6_tree_node_new_sibling (
   const struct ip_tree_node* const restrict src,
   const bool hot
) {
   struct ip_tree_node* node;

   node = ip6_tree_node_new_null();
   if ( node != NULL ) {
      node->hot = hot;
      ip6_addr_init_sibling ( &((node->addr).v6), &((src->addr).v6) );
   }

   return node;
}

/* new_child() */
struct ip_tree_node* ip4_tree_node_new_child (
   const struct ip_tree_node* const src,
   const bool bit_set,
   const bool hot
) {
   struct ip_tree_node* node;

   node = ip4_tree_node_new_null();
   if ( node != NULL ) {
      node->hot = hot;
      ip4_addr_init_child ( &((node->addr).v4), &((src->addr).v4), bit_set );
   }

   return node;
}

struct ip_tree_node* ip6_tree_node_new_child (
   const struct ip_tree_node* const restrict src,
   const bool bit_set,
   const bool hot
) {
   struct ip_tree_node* node;

   node = ip6_tree_node_new_null();
   if ( node != NULL ) {
      node->hot = hot;
      ip6_addr_init_child ( &((node->addr).v6), &((src->addr).v6), bit_set );
   }

   return node;
}


/* destroy() */
void ip4_tree_node_destroy (
   struct ip_tree_node** const restrict node_ptr
) {
   if ( *node_ptr != NULL ) {
      ip4_tree_node_destroy ( &((*node_ptr)->left) );    /* recursive */
      ip4_tree_node_destroy ( &((*node_ptr)->right) );   /* recursive */

      ip4_tree_node_init_null ( *node_ptr );  /* overwrite with null data */
      free ( *node_ptr );
      *node_ptr = NULL;
   }
}

void ip6_tree_node_destroy (
   struct ip_tree_node** const restrict node_ptr
) {
   if ( *node_ptr != NULL ) {
      ip6_tree_node_destroy ( &((*node_ptr)->left) );    /* recursive */
      ip6_tree_node_destroy ( &((*node_ptr)->right) );   /* recursive */

      ip6_tree_node_init_null ( *node_ptr );  /* overwrite with null data */
      free ( *node_ptr );
      *node_ptr = NULL;
   }
}


/* flip_addr_inplace() */
void ip4_tree_node_flip_addr_inplace ( struct ip_tree_node* const restrict node ) {
    ip4_addr_flip_inplace ( &((node->addr).v4) );
}

void ip6_tree_node_flip_addr_inplace ( struct ip_tree_node* const restrict node ) {
   ip6_addr_flip_inplace ( &((node->addr).v6) );
}
