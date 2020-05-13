#ifndef _HAVE_IP_TREE_OP_PURGE_H_
#define _HAVE_IP_TREE_OP_PURGE_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "../data/tree.h"

#include "../../ip.h"

int ip_tree_purge (
    struct ip_tree* const restrict tree,
    const ip_addr_variant_t* const restrict var_addr
);

#endif  /* _HAVE_IP_TREE_OP_PURGE_H_ */
