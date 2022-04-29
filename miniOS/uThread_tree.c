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

    if (!tree) {
        node = empty_tree();
        node->thread = thread;
        node->color = RED;
        node->v_time = v_time;
    } else {
        if (v_time < tree->v_time) {
            node = insert(thread, v_time, tree->left);
            if (!tree->left) {
                tree->left = node;
                node->parent = tree;
                tree->leftmost = node->leftmost;
                if (tree->color == RED)
                    recolor_on_insert(tree);
            }
        } else {
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
    if (node->left != NULL && node->right != NULL) {
        node->v_time = node->right->leftmost->v_time;
        return remove_node(node->right->leftmost, tree);
    } else {
        struct uThread_tree *new_node;
        enum color color = node->color;

        if (node->left != NULL) {
            node->left->parent = node->parent;
            new_node = node->left;
        } else if (node->right != NULL) {
            node->right->parent = node->parent;
            new_node = node->right;
        } else
            new_node = NULL;
        if (node->parent != NULL) {
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

        if (color == BLACK) {
            recolor_on_removal(new_node);
        }

        return tree;
    }
}

int recolor_on_insert(uThread_tree *tree) {
    if (!tree->parent)
        tree->color = BLACK;
    else {
        if (tree->parent->left->color == RED && tree->parent->right->color == RED) {
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
    if (tree->color == RED ||
        !tree->parent) // Soit le noeud à recolorier est à la racine ou est rouge : il devient noir
        tree->color = BLACK;
    else {
        // Si son frère est rouge, les enfants de celui-là sont noirs, avec une rotation on se ramène à un frère noir
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
        if (tree->parent->left == tree) { // Procédure symétrique suivant s'il s'agit du fils droit ou gauche
            if ((tree->parent->right->left == NULL || tree->parent->right->left->color == BLACK) &&
                (tree->parent->right->right == NULL || tree->parent->right->right->color == BLACK)) {
                tree->parent->right->color = RED;
                recolor_on_insert(tree->parent);
            } else {
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
        } else {
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

uThread_tree *rotate_left(uThread_tree *tree) {
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
