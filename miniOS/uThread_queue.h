//
//  uThread_queue.h
//  miniOS
//
//  Created by Alexi Canesse on 21/04/2022.
//

#ifndef uThread_queue_h
#define uThread_queue_h

#include <stdio.h>

#include "vCPU.h"

struct queue_element{
    uThread *element;
    struct queue_element *next;
};
typedef struct queue_element queue_element;

struct uThread_queue{
    queue_element *first;
    queue_element *last;
};
typedef struct uThread_queue uThread_queue;

int enqueue(uThread *thread);
uThread *dequeue(void);
uThread *pop_last(void);
uThread_queue* empty_queue(void);
#endif /* uThread_queue_h */
