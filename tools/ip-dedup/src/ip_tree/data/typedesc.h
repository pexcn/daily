#ifndef _HAVE_IP_TREE_DATA_TYPEDESC_H_
#define _HAVE_IP_TREE_DATA_TYPEDESC_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "node.h"
#include "../../ip.h"

/*
 * IP tree operations that depend on
 * whether IPv4 or IPv6 addresses are being processed
 * See node.h / node.c for the actual implementation.
 * */

/** Initializes an empty tree node */
typedef void (*ip_tree_node_init_null_func) ( struct ip_tree_node* const restrict );

/** Creates a new, empty tree node */
typedef struct ip_tree_node* (*ip_tree_node_new_null_func) (void);

/** Creates the sibling of the referenced node (bitflip at prefixlen) */
typedef struct ip_tree_node* (*ip_tree_node_new_sibling_func) (
   const struct ip_tree_node* const restrict, const bool
);

/** Creates a child of the referenced node */
typedef struct ip_tree_node* (*ip_tree_node_new_child_func) (
   const struct ip_tree_node* const restrict, const bool, const bool
);

/** Recursively frees a node and sets it to NULL */
typedef void (*ip_tree_node_destroy_func) ( struct ip_tree_node** const restrict );

/** Flips the bit at prefixlen for the given node */
typedef void (*ip_tree_node_flip_addr_inplace) ( struct ip_tree_node* const restrict );

/** Retrieves the prefixlen of an address object */
typedef ip_prefixlen_t (*ip_tree_node_op_get_addr_prefixlen) (
    const ip_addr_variant_t* const
);

/** Checks whether the bit at prefixlen is set in the given address object */
typedef bool (*ip_tree_node_op_check_addr_bit_set) (
    const ip_addr_variant_t* const, const ip_prefixlen_t
);


/** IP tree operations type descriptor */
struct ip_tree_typedesc {
    ip_prefixlen_t max_prefixlen;

    ip_tree_node_init_null_func         f_init_null;
    ip_tree_node_new_null_func          f_new_null;
    ip_tree_node_new_sibling_func       f_new_sibling;
    ip_tree_node_new_child_func         f_new_child;
    ip_tree_node_destroy_func           f_destroy;
    ip_tree_node_flip_addr_inplace      f_flip_addr_inplace;

    ip_tree_node_op_get_addr_prefixlen  f_get_addr_prefixlen;
    ip_tree_node_op_check_addr_bit_set  f_check_addr_bit_set;
};

/** IPv4 tree operations */
extern const struct ip_tree_typedesc IP4_TREE_NODE_OPS;

/** IPv6 tree operations */
extern const struct ip_tree_typedesc IP6_TREE_NODE_OPS;

#endif  /* _HAVE_IP_TREE_DATA_TYPEDESC_H_ */
