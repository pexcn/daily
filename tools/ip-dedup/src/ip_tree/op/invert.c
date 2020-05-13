#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#include "invert.h"

#include "../data/node.h"
#include "../data/typedesc.h"
#include "../data/tree.h"

#include "../../ip.h"


static int _ip_tree_invert (
   const struct ip_tree_typedesc* const restrict tdesc,
   struct ip_tree_node* const restrict node
);


int ip_tree_invert ( struct ip_tree* const restrict tree ) {
   if ( tree == NULL ) {
      errno = EINVAL;
      return -1;

   /* recursion checks next l/r nodes only, handle a few cases here:
    * - hot root : convert to empty tree
    * - empty root => empty tree : keep as-is
    * */
   } else if ( tree->root->hot ) {
      tree->tdesc->f_destroy ( &(tree->root->left) );
      tree->tdesc->f_destroy ( &(tree->root->right) );
      tree->root->hot = false;
      return 0;

   } else if ( (tree->root->left == NULL) && (tree->root->right == NULL) ) {
      return 0;

   } else {
      return _ip_tree_invert ( tree->tdesc, tree->root );
   }
}


static int _ip_tree_invert (
   const struct ip_tree_typedesc* const restrict tdesc,
   struct ip_tree_node* const restrict node
) {
   /*
    * the rule for inverting is quite simple: every hot node as an even hotter neighbor
    *
    * Assume the following collapsed tree with nodes a..e:
    *
    *               (a)
    *              /   \
    *             /     \
    *            /       \
    *          <b>       (c)
    *                    / \
    *                   /   \
    *                  /     \
    *                <d>      e
    *
    *
    * - nodes enclosed in parentheses () are transient -> a, c
    * - nodes enclosed in angle brackets are NULL      -> b, d
    * - nodes without a marker are ho                  -> e
    *
    * transient ::= at least one child node, never hot
    * NULL      ::= node does not exist, implies an existing sibling node
    * hot       ::= marked hot - leaf node, no child nodes
    *
    * When inverting this tree:
    *
    * - a remains transient
    * - b becomes hot
    *   -> create a new node
    * - c remains transient
    * - d becomes hot, e looses its marker
    *   -> move e to d, flipping the appropriate bit in its address
    *
    * => the inverted tree is already collapsed
    *
    *
    * Consider the following extended tree:
    *
    *               (X)
    *              /   \
    *             /     \
    *            /       \
    *           Y        (a)
    *                    / \
    *                   /   \
    *                  /     \
    *                <b>     (c)
    *                       /   \
    *                      /     \
    *                     /       \
    *                   <d>        e
    *
    * - transient ->  X, a, c
    * - NULL      ->  b, d
    * - hot       ->  Y, e
    *
    * When inverting this tree:
    *
    * - X remains transient
    * - Y looses its marker
    * - the inverse of Y gets pushed down the subtree rooted at a:
    *   - a remains transient
    *   - b becomes hot, because it is the inverse of Y minus c
    *   - c remains transient
    *   - d becomes hot, e looses its marker
    *
    * Again, the resulting inverse is already collapsed.
    *
    * => From a different point of view,
    *    inverting Y has no impact on the subtree rooted at (a).
    *    More generally, inverting any hot node does not affect its transient sibling.
    *
    *
    * Finally, transient nodes always remain transient, for instance:
    *
    *               (o)
    *              /   \
    *             /     \
    *            /       \
    *          (p)       (q)
    *         /   \     /   \
    *       <r>    s   t    <u>
    *
    * - transient ->  o, p, q
    * - NULL      ->  r, u
    * - hot       ->  s, t
    *
    * When inverting this tree:
    *
    * - o remains transient
    * - p remains transient
    * - q remains transient
    * - r becomes hot, s looses its marker
    * - u becomes hot, t looses its marker
    *
    * => The depth of the tree will not change.
    *    However, since NULL nodes get replaced with hot nodes,
    *    the overall number of elements is likely to increase.
    *
    * ------------------------------------------------------------------------
    *
    * So, for each node P at level k:
    *    (A)  if   left(P) and right(P) : invalid, a collapsed tree has no all-hot child nodes
    *
    *    (B)  elif left(P)
    *         (B-1)  if right is NULL   : move left to right, changing the appropriate bit
    *         (B-2)  else               : recurse for right
    *
    *    (C)  elif right(P)
    *         (C-1)  if left is NULL    : move right to left, changing the appropriate bit
    *         (C-)   else               : recurse for left
    *
    *    (D)  else foreach (left, right)
    *         (D-1)  if child node NULL : create hot node
    *         (D-2)  else               : recurse
    *
    * */

   if ( ip_tree_node_left_hot ( node ) ) {
      /* case (A) or (B-1/2) */
      if ( node->right != NULL ) {
         if ( node->right->hot ) {
            /* case (A): invalid tree */
            return -1;

         } else {
            /* case (B-2): destroy left, walk down right */
            tdesc->f_destroy ( &(node->left) );

            if ( _ip_tree_invert ( tdesc, node->right ) != 0 ) { return -1; }
         }

      } else {
         /* case (B-1): flip bit and move left to right  */
         tdesc->f_flip_addr_inplace ( node->left );
         node->right = node->left;
         node->left  = NULL;
      }

   } else if ( ip_tree_node_right_hot ( node ) ) {
      /* case (C) -- left is not hot due to previous branch */

      if ( node->left != NULL ) {
         /* case (C-2): destroy right, walk down left */
         tdesc->f_destroy ( &(node->right) );

         if ( _ip_tree_invert ( tdesc, node->left ) != 0 ) { return -1; }

      } else {
         /* case (C-1): flip bit and move right to left */
         tdesc->f_flip_addr_inplace ( node->right );
         node->left  = node->right;
         node->right = NULL;
      }

   } else {
      /* case (D) */
      /* when creating new nodes, copy from other node */

      if ( node->left == NULL ) {
         if ( node->right == NULL ) { /* invalid tree */ return -1; }

         /* case (D-1): create hot node */
         node->left = tdesc->f_new_sibling ( node->right, true );
         if ( node->left == NULL ) { return -1; }

      /* case (D-2): recurse */
      } else if ( _ip_tree_invert ( tdesc, node->left ) != 0 ) {
         return -1;
      }

      if ( node->right == NULL ) {
         /* case (D-1): create hot node */
         node->right = tdesc->f_new_sibling ( node->left, true );
         if ( node->right == NULL ) { return -1; }

      /* case (D-2): recurse */
      } else if ( _ip_tree_invert ( tdesc, node->right ) != 0 ) {
         return -1;
      }
   }

   return 0;
}
