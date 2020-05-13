#ifndef _HAVE_IP_TREE_OP_COLLAPSE_H_
#define _HAVE_IP_TREE_OP_COLLAPSE_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "../data/tree.h"

/**
 * Collapses an IP tree by recursively merging hot subnets
 * into their supernet and deleting overshadowed subnets.
 *
 * Note: bottom-up DFS with lookahead
 *
 * @param tree:  tree  (should not be NULL)
 *
 * @returns 0 on success, any other value indicates an error
 * */
int ip_tree_collapse ( struct ip_tree* const restrict tree );

#endif  /* _HAVE_IP_TREE_OP_COLLAPSE_H_ */
