#ifndef _HAVE_DATA_IP_TREE_DATA_NODE_H_
#define _HAVE_DATA_IP_TREE_DATA_NODE_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "../../ip.h"

/**
 * IPv4 subnet tree node
 *
 * Consists of an ip address (addr) and a netmask length (prefixlen).
 *
 * The purpose of this data structure is merging and deduplicating IP networks,
 * e.g. for aggregating blocklists or routes.
 * Each level of the tree represents a single bit in the network address,
 * alongside with the appropriate netmask length for that particular level.
 *
 * A network can be split into two subnets,
 * except for the leaf case (IPv4 prefixlen == 32).
 * These subnets may be reached via the 'left' or 'right' attribute,
 * depending on whether the subnet's bit is set to zero or one, respectively.
 *
 * The 'hot' attribute indicates whether network of a node is relevant,
 * i.e. has been explicitly added to tree or is the result of merging/dedup.
 * If the 'hot' attribute is set to false, then this node exists solely
 * for some 'hot' subnet at a deeper level. *
 *
 * Suppose you add "192.168.128.0/18" (128 + 0 in the 3rd block) to the tree.
 * This creates 17 transient nodes for representing "192.168.128.0/17"
 * and creates a 'hot' child node for "192.168.128.0/18",
 * stored in the 'left' attr of the "192.168.128.0/17" node.
 *
 * Afterwards you add "192.168.192.0/18" (128 + 64 in the 3rd block).
 * This will reuse the first transient 17 nodes because the bit sequence is the same.
 * A new 'hot' child node at level 18 gets created for "192.168.192.0/18",
 * which gets stored in the 'right' attr of the "192.168.128.0/17" node.
 *
 * Once more you add "192.168.240.0/20". No new nodes will be created
 * since this network is overshadowed by the "192.168.192.0/18" node.
 *
 * Then you call ip4_tree_collapse() on the data structure.
 * It will detect that both subnets of the "192.168.128.0/17" node are 'hot'
 * and merges them by marking the "192.168.128.0/17" node as 'hot'.
 * The subnets are not of use anymore and their data gets freed.
 *
 * Finally, you add "192.168.0.0/16".
 * This will mark the existing "192.168.0.0/16" node as 'hot'
 * and free() all overshadowed subnetworks, including "192.168.128.0/17".
 * */
struct ip_tree_node;
struct ip_tree_node {
   struct ip_tree_node* left;
   struct ip_tree_node* right;

   bool                 hot;
   ip_addr_variant_t    addr;
};


/**
 * @param node  IP tree node (not NULL)
 *
 * @return true if (node->left is not NULL and node->left->hot), else False.
 * */
bool ip_tree_node_left_hot ( const struct ip_tree_node* const restrict node );


/**
 * @param node  IP tree node (not NULL)
 *
 * @return true if (node->right is not NULL and node->right->hot), else False.
 * */
bool ip_tree_node_right_hot ( const struct ip_tree_node* const restrict node );


/* IPv4 functions */
/**
 * Initializes an IP tree node representating an IPv4 address with null data.
 *
 * @param node  IP tree node (not NULL)
 *
 * @return None (implicit)
 * */
void ip4_tree_node_init_null ( struct ip_tree_node* const restrict node );


/**
 * Creates a new IP tree node representing an IPv4 address
 * and initializes it with null data.
 *
 * @return new IP tree node, NULL on error (malloc())
 * */
struct ip_tree_node* ip4_tree_node_new_null (void);


/**
 * Creates a sibling of the given IPv4 tree node.
 *
 * The sibling's address is mostly identical to the input node,
 * but has its bit flipped at prefixlen.
 *
 * @param src  original node for which a sibling should be created (not NULL)
 * @param hot  whether the created sibling should be marked as hot or not
 *
 * @return new IP tree node, NULL on error (malloc())
 * */
struct ip_tree_node* ip4_tree_node_new_sibling (
   const struct ip_tree_node* const restrict src,
   const bool hot
);


/**
 * Creates a child of the given IPv4 tree node.
 *
 * The child inherits its parent's network address, but extends the
 * prefixlen by 1, thus creating a subnet. The value of the bit at
 * that position can be controlled with the bit_set parameter.
 *
 * @param src      original node for which a child should be created (not NULL)
 * @param bit_set  whether the subnet's bit should be set
 * @param hot      whether the child node should be marked as hot
 *
 * @return new IP tree node, NULL on error (malloc())
 * */
struct ip_tree_node* ip4_tree_node_new_child (
   const struct ip_tree_node* const restrict src,
   const bool bit_set,
   const bool hot
);


/**
 * Recursively destroys a IP tree containing IPv4 addresses.
 *
 * @param node_ptr   pointer to tree (may be NULL),
 *                   the tree will be freed and the pointer will be set to NULL
 *
 * @return None (implicit)
 * */
void ip4_tree_node_destroy ( struct ip_tree_node** const restrict node_ptr );


/**
 * Flips the address bit if a node at its prefixlen,
 * thus turning the node into its sibling.
 * (And vice versa, should you call this function on that node again.)
 *
 * @param node  IP tree node (not NULL)
 *
 * @return None (implicit)
 */
void ip4_tree_node_flip_addr_inplace (
   struct ip_tree_node* const restrict node
);


/* IPv6 functions */
/**
 * Initializes an IP tree node representating an IPv6 address with null data.
 *
 * @param node  IP tree node (not NULL)
 *
 * @return None (implicit)
 * */
void ip6_tree_node_init_null ( struct ip_tree_node* const restrict node );


/**
 * Creates a new IP tree node representing an IPv6 address
 * and initializes it with null data.
 *
 * @return new IP tree node, NULL on error (malloc())
 * */
struct ip_tree_node* ip6_tree_node_new_null (void);


/**
 * Creates a sibling of the given IPv6 tree node.
 *
 * The sibling's address is mostly identical to the input node,
 * but has its bit flipped at prefixlen.
 *
 * @param src  original node for which a sibling should be created (not NULL)
 * @param hot  whether the created sibling should be marked as hot or not
 *
 * @return new IP tree node, NULL on error (malloc())
 * */
struct ip_tree_node* ip6_tree_node_new_sibling (
   const struct ip_tree_node* const restrict src,
   const bool hot
);


/**
 * Creates a child of the given IPv6 tree node.
 *
 * The child inherits its parent's network address, but extends the
 * prefixlen by 1, thus creating a subnet. The value of the bit at
 * that position can be controlled with the bit_set parameter.
 *
 * @param src      original node for which a child should be created (not NULL)
 * @param bit_set  whether the subnet's bit should be set
 * @param hot      whether the child node should be marked as hot
 *
 * @return new IP tree node, NULL on error (malloc())
 * */
struct ip_tree_node* ip6_tree_node_new_child (
   const struct ip_tree_node* const restrict src,
   const bool bit_set,
   const bool hot
);


/**
 * Recursively destroys a IP tree containing IPv6 addresses.
 *
 * @param node_ptr   pointer to tree (may be NULL),
 *                   the tree will be freed and the pointer will be set to NULL
 *
 * @return None (implicit)
 * */
void ip6_tree_node_destroy (
   struct ip_tree_node** const restrict node_ptr
);


/**
 * Flips the address bit if a node at its prefixlen,
 * thus turning the node into its sibling.
 * (And vice versa, should you call this function on that node again.)
 *
 * @param node  IP tree node (not NULL)
 *
 * @return None (implicit)
 */
void ip6_tree_node_flip_addr_inplace (
   struct ip_tree_node* const restrict node
);

#endif
