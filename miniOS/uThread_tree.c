//
//  uThread_tree.c
//  miniOS
//
//
#include "uThread_tree.h"

#include <stdio.h>
#include <stdlib.h>

uThread_tree *empty_tree(void){
    uThread_tree *tree = (uThread_tree *) malloc(sizeof(uThread_tree));
    if(tree == NULL){//malloc failed
        perror("Failed to create an empty tree.\n");
        return NULL;
    }
    
    tree->color = BLACK; //a leaf is black
    tree->parent = NULL;
    tree->left = NULL;
    tree->right = NULL;
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
                if (tree->color == RED)
                    recolor(tree);
            }
        } else {
            node = insert(thread, v_time, tree->right);
            if (!tree->right) {
                tree->right = node;
                node->parent = tree;
                if (tree->color == RED)
                    recolor(tree);
            }
        }
    }

    return node;
}

int recolor(uThread_tree *tree) {
    if (!tree->parent)
        tree->color = BLACK;
    else {
        if (tree->parent->left->color == RED && tree->parent->right->color == RED) {
            tree->parent->left->color = BLACK;
            tree->parent->right->color = BLACK;
            tree->parent->color = RED;
            if (tree->parent->parent != NULL && tree->parent->parent->color == RED)
                recolor(tree->parent->parent);
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

uThread_tree *rotate_right(uThread_tree *tree) {
    uThread_tree *right = tree, *parent = tree->parent, *left = tree->left, *left_left = tree->left->left;
    left->parent = parent;
    if (parent->left == right)
        parent->left = left;
    else
        parent->right = left;
    left_left->parent = right;
    right->left = left_left;
    right->parent = left;
    left->right = right;
    return left;
}

uThread_tree *rotate_left(uThread_tree *tree) {
    uThread_tree *left = tree, *parent = tree->parent, *right = tree->right, *right_right = tree->right->right;
    right->parent = parent;
    if (parent->left == left)
        parent->left = right;
    else
        parent->right = right;
    right_right->parent = left;
    left->right = right_right;
    left->parent = right;
    right->left = left;
    return right;
}
