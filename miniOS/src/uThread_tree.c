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
    tree->v_time = 0;

    return tree;
}

uThread_tree *insert(uThread *thread, long int v_time, uThread_tree *tree) {
    uThread_tree *node;

    if (!tree) { // If tree is empty, create one
        node = empty_tree();
        node->thread = thread;
        node->color = RED;
        node->v_time = v_time;
    } else {
        if (v_time < tree->v_time) { // Find recursively where to insert
            node = insert(thread, v_time, tree->left);
            if (!tree->left) { // If the node is to be inserted as a child
                tree->left = node;
                node->parent = tree;
                tree->leftmost = node->leftmost;
                if (tree->color == RED)
                    recolor_on_insert(tree); // Re-equilibrate if both node and its parent are red
            }
        } else { // Symmetric
            node = insert(thread, v_time, tree->right);
            if (!tree->right) {
                tree->right = node;
                node->parent = tree;
                if (tree->color == RED)
                    recolor_on_insert(tree);
            }
        }
    }

    return node;
}

uThread_tree *remove_node(uThread_tree *node, uThread_tree *tree) {
    if (node->left != NULL && node->right != NULL) { // If node has 2 children, swap its value with one at the bottom
        node->v_time = node->right->leftmost->v_time;
        return remove_node(node->right->leftmost, tree);
    } else {
        struct uThread_tree *new_node;
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
            if (!node->parent->left)
                node->parent->leftmost = node->parent;
            else
                node->parent->leftmost = node->parent->left->leftmost;
        } else
            tree = new_node;
        free(node);

        if (!tree)
            return tree;

        if (color == BLACK) { // If node was black, tree has to be re-equilibrated
            recolor_on_removal(new_node);
        }

        return tree;
    }
}

int recolor_on_insert(uThread_tree *tree) {
    if (!tree->parent) // Root of tree can become black without further questions
        tree->color = BLACK;
    else {
        if (tree->parent->left->color == RED && tree->parent->right->color == RED) { // If both childs are red, simple
            tree->parent->left->color = BLACK;
            tree->parent->right->color = BLACK;
            tree->parent->color = RED;
            if (tree->parent->parent != NULL && tree->parent->parent->color == RED)
                recolor_on_insert(tree->parent->parent);
        } else {
            if (tree->parent->left->color == RED && tree->left->color == BLACK)
                tree = rotate_left(tree);
            else if (tree->parent->right->color == RED && tree->right->color == BLACK)
                tree = rotate_right(tree);
            tree = tree->parent;
            if (tree->left->left->color == RED)
                tree = rotate_right(tree);
            else if (tree->right->right->color == RED)
                tree = rotate_left(tree);
            tree->color = BLACK;
            tree->left->color = RED;
            tree->right->color = RED;
        }
    }

    return 0;
}

int recolor_on_removal(uThread_tree *tree) {
    if (tree->color == RED || !tree->parent) // Father of the removed node become black if it was red / is the root
        tree->color = BLACK;
    else {
        // If brother is red, its children are black, using a rotation it becomes black
        if (tree->parent->left->color == RED) {
            rotate_right(tree->parent);
            tree->parent->color = RED;
            tree->parent->parent->color = BLACK;
        }
        if (tree->parent->right->color == RED) {
            rotate_left(tree->parent);
            tree->parent->color = RED;
            tree->parent->parent->color = BLACK;
        }
        if (tree->parent->left == tree) {
            // Both children of brother are black -> it becomes red and the problem goes up to the parent
            if ((tree->parent->right->left == NULL || tree->parent->right->left->color == BLACK) &&
                (tree->parent->right->right == NULL || tree->parent->right->right->color == BLACK)) {
                tree->parent->right->color = RED;
                recolor_on_insert(tree->parent);
            } else {
                // If left children of brother is red and right is black, with a rotation the left becomes red
                if (tree->parent->right->left != NULL && tree->parent->right->left->color == RED &&
                    (tree->parent->right->right == NULL || tree->parent->right->right->color == BLACK)) {
                    rotate_right(tree->parent->right);
                    tree->parent->right->color = BLACK;
                    tree->parent->right->right->color = RED;
                }
                rotate_left(tree->parent);
                tree->parent->parent->color = tree->parent->color;
                tree->parent->color = BLACK;
                tree->parent->parent->right->color = BLACK;
                tree->color = BLACK;
            }
        } else { // Symmetric
            if ((tree->parent->left->right == NULL || tree->parent->left->right->color == BLACK) &&
                (tree->parent->left->left == NULL || tree->parent->left->left->color == BLACK)) {
                tree->parent->left->color = RED;
                recolor_on_insert(tree->parent);
            } else {
                if (tree->parent->left->right != NULL && tree->parent->left->right->color == RED &&
                    (tree->parent->left->left == NULL || tree->parent->left->left->color == BLACK)) {
                    rotate_left(tree->parent->left);
                    tree->parent->left->color = BLACK;
                    tree->parent->left->left->color = RED;
                }
                rotate_right(tree->parent);
                tree->parent->parent->color = tree->parent->color;
                tree->parent->color = BLACK;
                tree->parent->parent->left->color = BLACK;
                tree->color = BLACK;
            }
        }
    }

    return 0;
}

uThread_tree *rotate_right(uThread_tree *tree) {
    uThread_tree *right = tree, *parent = tree->parent, *left = tree->left, *left_right = tree->left->right;
    left->parent = parent;
    if (parent->left == right)
        parent->left = left;
    else
        parent->right = left;
    if (left_right != NULL) {
        left_right->parent = right;
        right->leftmost = left_right->leftmost;
    } else
        right->leftmost = right;
    right->left = left_right;
    right->parent = left;
    left->right = right;
    return left;
}

uThread_tree *rotate_left(uThread_tree *tree) { // Symmetric
    uThread_tree *left = tree, *parent = tree->parent, *right = tree->right, *right_left = tree->right->left;
    right->parent = parent;
    if (parent->left == left)
        parent->left = right;
    else
        parent->right = right;
    right_left->parent = left;
    left->right = right_left;
    left->parent = right;
    right->left = left;
    right->leftmost = left->leftmost;
    return right;
}