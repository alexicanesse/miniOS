//
//  uThread_tree.c
//  miniOS
//
//
#include "uThread_tree.h"

#include <stdio.h>
#include <stdlib.h>

uThread_tree *empty_tree(void) {
    uThread_tree *tree = (uThread_tree *) malloc(sizeof(uThread_tree));
    if (tree == NULL) {//malloc failed
        perror("Failed to create an empty tree.\n");
        return NULL;
    }

    tree->color = BLACK; //a leaf is black
    tree->parent = NULL;
    tree->left = NULL;
    tree->right = NULL;
    tree->leftmost = tree;
    tree->thread = NULL;

    return tree;
}

// Returns a pointer to the updated tree root
uThread_tree *insert(uThread *thread, uThread_tree *tree) {
    uThread_tree *node;

    if (!tree) { // If tree is empty, create one
        node = empty_tree();
        node->thread = thread;
        node->color = RED;
        node->leftmost = node;
    } else {
        if (thread->vTime < tree->thread->vTime) { // find recursively where to insert
            node = insert(thread, tree->left);
            // If the subtree that should contain the new node is empty, attach the node here
            if (node->thread == thread) {
                tree->left = node;
                node->parent = tree;
                update_leftmost(tree);
                if (tree->color == RED)
                    recolor_on_insert(tree); // Re-equilibrate if both node and its parent are red
            }
        } else { // Symmetric
            node = insert(thread, tree->right);
            if (node->thread == thread) {
                tree->right = node;
                node->parent = tree;
                if (tree->color == RED)
                    recolor_on_insert(tree);
            }
        }
    }

    // Two possibilities, either node is already the root or it is the one we just inserted
    return get_root(node);
}

uThread_tree *remove_node(uThread_tree *node, uThread_tree *tree) {
    // If node has 2 children, swap its value with one at the bottom
    // (should not happen as we only remove leftmost nodes)
    if (node->left != NULL && node->right != NULL) {
        node->thread = node->right->leftmost->thread;
        printf("Wait no\n");
        return remove_node(node->right->leftmost, tree);
    } else { // Else replace the node by its only child or by a leaf if there is none
        struct uThread_tree *new_node, *any_node;
        enum color color = node->color;

        // Update potential child parent and select it as the node that will replace the one to be removed
        if (node->left != NULL) {
            node->left->parent = node->parent;
            new_node = node->left;
        } else if (node->right != NULL) {
            node->right->parent = node->parent;
            new_node = node->right;
        } else
            new_node = NULL;
        if (node->parent != NULL) { // If node has parents, update their children (+pointer to leftmost)
            if (node == node->parent->left)
                node->parent->left = new_node;
            else
                node->parent->right = new_node;
            update_leftmost(node->parent);
        } else // Else it was the root
            tree = new_node;
        // If node was black, tree has to be re-equilibrated
        if (color == BLACK && node->parent != NULL)
            recolor_on_removal(new_node, node->parent);
        any_node = !new_node ? node->parent : new_node; // get non-NULL node of the tree (if the tree is not empty)

        free(node);

        return !any_node ? any_node : get_root(any_node);
    }
}

// Happens when a node and one of its children are both red. The argument is the parent node
int recolor_on_insert(uThread_tree *tree) {
    if (!tree->parent) // Root of tree can become black without further questions
        tree->color = BLACK;
    else {
        /* If both node and its sibling are red, move the problem one rank up, to the parent (*tree in uppercase)
         *
         *                          ?
         *                          |
         *         b                r
         *         /\              /\
         *        R  r    -->     B  b
         *       /\  /\          /\  /\
         *      r               r
         *
         */
        if (get_color(tree->parent->left) == RED && get_color(tree->parent->right) == RED) {
            tree->parent->left->color = BLACK;
            tree->parent->right->color = BLACK;
            tree->parent->color = RED;
            if (get_color(tree->parent->parent) == RED)
                recolor_on_insert(tree->parent->parent);
        } else {
            /* Here brother is necessarily black
             * If the node is not the child of the same side of its parent as its problematic child,
             * rotate to change that:
             *
             *           b             b
             *          /\            /\
             *         R  b   -->    r  b  -->  *tree is updated to the new parent one
             *        /\            /\
             *          r          R
             */
            if (get_color(tree->parent->left) == RED && get_color(tree->left) == BLACK)
                tree = rotate_left(tree);
            else if (get_color(tree->parent->right) == RED && get_color(tree->right) == BLACK)
                tree = rotate_right(tree);
            tree = tree->parent;
            /* Rotate on the parent and color it red to end the function
             *
             *        B            R             B
             *       /\           /\            /\
             *      r  b  -->    r  b   -->    r  r
             *     /\               /\
             *    r                   b
             *
             */
            if (get_color(tree->left) == RED) // Find which subtree needs to be fixed
                tree = rotate_right(tree);
            else if (get_color(tree->right) == RED)
                tree = rotate_left(tree);
            tree->color = BLACK;
            tree->left->color = RED;
            tree->right->color = RED;
        }
    }

    return 0;
}

// Happens when a black node is removed, node is the replacement node, parent its parent
int recolor_on_removal(uThread_tree *node, uThread_tree *parent) {
    // Replacement node stay black if it is the root and become black if it was red
    // In that case, there is no "double black" node
    if (get_color(node) == RED || !parent) {
        if (node != NULL)
            node->color = BLACK;
    } else {
        /* We go back to the case where sibling is black (*node in uppercase):
         * If sibling is red, parent is black and we just do a rotation
         *
         *      b             r            b
         *     /\            /\           /\
         *    B  r   -->    b  b  -->    r  b
         *       /\        /\           /\
         *      b  b      B  b         B  b
         *
         */
        if (get_color(parent->left) == RED) {
            // We do not update the "parent" variable as we want it to stay the parent of "node"
            rotate_right(parent);
            parent->color = RED;
            parent->parent->color = BLACK;
        }
        if (get_color(parent->right) == RED) { // Symmetric
            rotate_left(parent);
            parent->color = RED;
            parent->parent->color = BLACK;
        }
        if (parent->left == node) { // NB: the sibling cannot be a leaf (easy to demonstrate)
            // Both children of sibling are black -> it becomes red and the problem goes up to the parent
            if (get_color(parent->right->left) == BLACK && get_color(parent->right->right) == BLACK) {
                parent->right->color = RED;
                recolor_on_removal(parent, parent->parent);
            } else {
                /* We go back to the case where right child of sibling is red
                 * If left children of brother is red and right is black, with a rotation the left becomes red
                 *
                 *        parent      parent        parent
                 *          /\          /\            /\
                 *         B  b   -->  B  r          B  b
                 *            /\          /\    -->     /\
                 *           r  b           b             r
                 *                          /\            /\
                 *                            b             b
                 */
                if (get_color(parent->right->left) == RED && get_color(parent->right->right) == BLACK) {
                    rotate_right(parent->right);
                    parent->right->color = BLACK;
                    parent->right->right->color = RED;
                }
                /* Sibling is black and its right child is red
                 * With a rotation, we "add" an ancestor that we set to black to the node (parent color is "x")
                 *
                 *       x              b              x
                 *      /\             /\             /\
                 *     B  b    -->    x  r    -->    b  b
                 *        /\         /\             /\
                 *          r       B              B
                 *
                 */
                rotate_left(parent);
                parent->parent->color = parent->color;
                parent->color = BLACK;
                parent->parent->right->color = BLACK;
            }
        } else { // Symmetric
            if (get_color(parent->left->right) == BLACK && get_color(parent->left->left) == BLACK) {
                parent->left->color = RED;
                recolor_on_removal(parent, parent->parent);
            } else {
                if (get_color(parent->left->right) == RED && get_color(parent->left->left) == BLACK) {
                    rotate_left(parent->left);
                    parent->left->color = BLACK;
                    parent->left->left->color = RED;
                }
                rotate_right(parent);
                parent->parent->color = node->parent->color;
                parent->color = BLACK;
                parent->parent->left->color = BLACK;
            }
        }
    }

    return 0;
}

uThread_tree *rotate_right(uThread_tree *tree) {
    /* Initial situation:
     *
     *          parent
     *            |
     *          right
     *            /\
     *          /    \
     *        left    rr
     *         /\
     *       ll  lr
     */
    uThread_tree *right = tree, *left = tree->left;
    /* Change parent child:
     *
     *          parent              right
     *            |                  /\
     *           left             left rr
     *            /\
     *          ll  lr
     */
    if (right->parent != NULL) {
        if (right == right->parent->left)
            right->parent->left = left;
        else
            right->parent->right = left;
    }
    left->parent = right->parent; // No rotation with empty node
    /* Attach initial node in exchange for the new right subtree
     *
     *          parent
     *            |
     *           left
     *            /\
     *          /    \
     *        ll    right
     *                /\
     *              lr  rr
     */

    right->left = left->right;
    if (left->right != NULL) {
        left->right->parent = right;
        right->leftmost = left->right->leftmost;
    } else
        right->leftmost = right;
    left->right = right;
    right->parent = left;

    return left;
}

uThread_tree *rotate_left(uThread_tree *tree) { // Symmetric
    /* Initial situation:
     *
     *          parent
     *            |
     *          left
     *            /\
     *          /    \
     *        lr    right
     *               /\
     *             rl  rr
     */
    uThread_tree *left = tree, *right = tree->right;
    /* Change parent child:
     *
     *          parent              left
     *            |                  /\
     *          right              lr right
     *            /\
     *          rl  rr
     */
    if (left->parent != NULL) {
        if (left == left->parent->left)
            left->parent->left = right;
        else
            left->parent->right = right;
    }
    right->parent = left->parent; // No rotation with empty node
    /* Attach initial node in exchange for the new right subtree
     *
     *          parent
     *            |
     *          right
     *           /\
     *         /    \
     *      left     rr
     *       /\
     *     lr  rl
     */

    left->right = right->left;
    if (right->left != NULL)
        right->left->parent = right;
    right->left = left;
    right->leftmost = left->leftmost;
    left->parent = right;

    return right;
}

enum color get_color(uThread_tree *node) {
    if (!node)
        return BLACK;
    return node->color;
}

uThread_tree *get_root(uThread_tree *node) {
    if (!node->parent)
        return node;
    return get_root(node->parent);
}

int update_leftmost(uThread_tree *node) {
    if (node->leftmost != (!node->left ? node : node->left->leftmost)) {
        node->leftmost = !node->left ? node : node->left->leftmost;

        if (node->parent != NULL && node == node->parent->left)
            update_leftmost(node->parent);
    }

    return 0;
}
