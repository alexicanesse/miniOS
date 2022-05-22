//
//  uThread_tree.h
//  miniOS
//
//

#ifndef uThread_tree_h
#define uThread_tree_h

#include "vCPU.h"

#include <stdio.h>

enum color { RED, BLACK };

struct uThread_tree{
    uThread *thread;
    enum color color; //red <-> 0 && black <-> 1
    struct uThread_tree *parent;
    struct uThread_tree *left;
    struct uThread_tree *right;
    struct uThread_tree *leftmost;
};
typedef struct uThread_tree uThread_tree;

uThread_tree *empty_tree(void);

uThread_tree *insert(uThread *thread, uThread_tree *tree);

uThread_tree * recolor_on_insert(uThread_tree *tree);

uThread_tree * recolor_on_removal(uThread_tree *tree);

uThread_tree *rotate_right(uThread_tree *tree);

uThread_tree *rotate_left(uThread_tree *tree);

uThread_tree *remove_node(uThread_tree *node, uThread_tree *tree);

uThread_tree *get_root(uThread_tree *node);

enum color get_color(uThread_tree *node);

int update_leftmost(uThread_tree *node);

#endif /* uThread_tree_h */
