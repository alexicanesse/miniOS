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
    long int v_time;
};
typedef struct uThread_tree uThread_tree;

uThread_tree *empty_tree(void);

uThread_tree *insert(uThread *thread, long int v_time, uThread_tree *tree);

int recolor_on_insert(uThread_tree *tree);

int recolor_on_removal(uThread_tree *tree);

uThread_tree *rotate_right(uThread_tree *tree);

uThread_tree *rotate_left(uThread_tree *tree);


#endif /* uThread_tree_h */
