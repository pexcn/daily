#ifndef _HAVE_IP_TREE_OP_INVERT_H_
#define _HAVE_IP_TREE_OP_INVERT_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "../data/tree.h"


/**
 * Inverts a tree so that every hot node looses its 'hot' marker and vice versa.
 *
 * injective operation except for:
 * - hot root node (0/0)  -> empty tree
 * - empty tree          <-> empty tree
 *
 * Note: top-down DFS
 *
 * @param tree:  collapsed tree (should not be NULL)
 *
 * @return non-zero on error (invalid input tree or malloc() failure)
 * */
int ip_tree_invert ( struct ip_tree* const restrict tree );


#endif  /* _HAVE_IP_TREE_OP_INVERT_H_ */
