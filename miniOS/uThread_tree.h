//
//  uThread_tree.h
//  miniOS
//
//  Created by Alexi Canesse on 21/04/2022.
//

#ifndef uThread_tree_h
#define uThread_tree_h

#include "vCPU.h"

#include <stdio.h>

enum color { RED, BLACK };

struct uThread_tree{
    uThread *thread;
    enum color color; //red <-> 0 && black <-> 1
    struct uThread_tree *left;
    struct uThread_tree *right;
};
typedef struct uThread_tree uThread_tree;

uThread_tree *empty_tree(void);


#endif /* uThread_tree_h */
