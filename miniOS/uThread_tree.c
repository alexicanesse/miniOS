//
//  uThread_tree.c
//  miniOS
//
//  Created by Alexi Canesse on 21/04/2022.
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
    tree->left = NULL;
    tree->right = NULL;
    tree->thread = NULL;
    
    return tree;
}
