#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "typedesc.h"
#include "node.h"
#include "../../ip.h"


static ip_prefixlen_t ip4_tree_node_op_get_addr_prefixlen (
    const ip_addr_variant_t* const restrict addr
);

static bool ip4_tree_node_op_check_addr_bit_set (
    const ip_addr_variant_t* const restrict addr,
    const ip_prefixlen_t prefixpos
);


static ip_prefixlen_t ip6_tree_node_op_get_addr_prefixlen (
    const ip_addr_variant_t* const restrict addr
);

static bool ip6_tree_node_op_check_addr_bit_set (
    const ip_addr_variant_t* const restrict addr,
    const ip_prefixlen_t prefixpos
);


const struct ip_tree_typedesc IP4_TREE_NODE_OPS = {
    .max_prefixlen          = IP4_MAX_PREFIXLEN,

    .f_init_null            = ip4_tree_node_init_null,
    .f_new_null             = ip4_tree_node_new_null,
    .f_new_sibling          = ip4_tree_node_new_sibling,
    .f_new_child            = ip4_tree_node_new_child,
    .f_destroy              = ip4_tree_node_destroy,
    .f_flip_addr_inplace    = ip4_tree_node_flip_addr_inplace,

    .f_get_addr_prefixlen   = ip4_tree_node_op_get_addr_prefixlen,
    .f_check_addr_bit_set   = ip4_tree_node_op_check_addr_bit_set
};


const struct ip_tree_typedesc IP6_TREE_NODE_OPS = {
    .max_prefixlen          = IP6_MAX_PREFIXLEN,

    .f_init_null            = ip6_tree_node_init_null,
    .f_new_null             = ip6_tree_node_new_null,
    .f_new_sibling          = ip6_tree_node_new_sibling,
    .f_new_child            = ip6_tree_node_new_child,
    .f_destroy              = ip6_tree_node_destroy,
    .f_flip_addr_inplace    = ip6_tree_node_flip_addr_inplace,

    .f_get_addr_prefixlen   = ip6_tree_node_op_get_addr_prefixlen,
    .f_check_addr_bit_set   = ip6_tree_node_op_check_addr_bit_set
};



static ip_prefixlen_t ip4_tree_node_op_get_addr_prefixlen (
    const ip_addr_variant_t* const restrict addr
) {
    return (addr->v4).prefixlen;
}

static bool ip4_tree_node_op_check_addr_bit_set (
    const ip_addr_variant_t* const restrict addr,
    const ip_prefixlen_t prefixpos
) {
    return ip4_addr_bit_is_set_at ( &(addr->v4), prefixpos );
}


static ip_prefixlen_t ip6_tree_node_op_get_addr_prefixlen (
    const ip_addr_variant_t* const restrict addr
) {
    return (addr->v6).prefixlen;
}

static bool ip6_tree_node_op_check_addr_bit_set (
    const ip_addr_variant_t* const restrict addr,
    const ip_prefixlen_t prefixpos
) {
    return ip6_addr_bit_is_set_at ( &(addr->v6), prefixpos );
}
