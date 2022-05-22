//
//  uThread_queue.c
//  miniOS
//
//  Created by Alexi Canesse on 21/04/2022.
//

#include <stdlib.h>

#include "uThread_queue.h"
#include "miniOS.h"

#ifdef WITH_OWN_HMM
extern void* malloc(size_t size);
extern void* realloc(void* ptr, size_t size);
extern void free(void* ptr);
#endif

extern uThread_queue *queue;
extern pthread_mutex_t mutex;


int enqueue(uThread *thread){
    queue_element *element = (queue_element *) malloc(sizeof(queue_element));
    if(element == NULL) /* if malloc failed */
        return -1; /* errno is already set by malloc */
    
    /* init element */
    element->element = thread;
    element->next = NULL;
    
    /* add element to the queue at the end */
    pthread_mutex_lock(&mutex); /* other threads are not able to acces protects ressources utile we release the lock */
    if(queue == NULL)
        queue = empty_queue();
    
    if(queue->first == NULL)
        queue->first = element;
    else
        queue->last->next = element;
    queue->last = element;
    pthread_mutex_unlock(&mutex);
    
    return 0;
}

uThread *dequeue(void){
    uThread *thread = queue->first->element; /* next thread */

    pthread_mutex_lock(&mutex); /* other threads are not able to acces protects ressources utile we release the lock */
    if(queue->first == queue->last){ /* there is only one node */
        queue->first = NULL;
        queue->last = NULL;
    }
    else
        queue->first = queue->first->next;
    pthread_mutex_unlock(&mutex);

    
    return thread;
}


uThread_queue* empty_queue(void){
    uThread_queue *queue = (uThread_queue *) malloc(sizeof(uThread_queue));
    queue->first = NULL;
    queue->last = NULL;
    return queue;
}

