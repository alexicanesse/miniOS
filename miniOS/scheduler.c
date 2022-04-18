//
//  scheduler.c
//  miniOS
//
//  Created on 15/04/2022.
//
#include <stdlib.h>

#include "scheduler.h"
#include "vCPU.h"
#include "miniOS_private.h"


uThread_queue *queue;



enum scheduler_policies policy = RR;

scheduler_type scheduler;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; //lock use to make some ressources thread-safe

void config_scheduler(int quantum, enum scheduler_policies scheduler_policy){
    scheduler.quantum = quantum;
    
    if(scheduler_policy == CFS){
        policy = CFS;
        scheduler.func = CFS_func;
    }
    else{ //RR
        scheduler.func = RR_func;
        policy = RR;
        queue->first = NULL;
        queue->last = NULL;
    }
}

uThread *RR_func(void){ //we update the queue and switch to the next uThread
#warning need to schedule idle if empty queue
    uThread *thread = queue->first->element; //next thread

    pthread_mutex_lock(&mutex); //other threads are not able to acces protects ressources utile we release the lock
    queue->last->next = queue->first;
    queue->last = queue->first; //we put it at the end
    queue->first = queue->first->next; //we remove it from the begining
    queue->last->next = NULL;
    pthread_mutex_unlock(&mutex);
    
    return thread;
}

uThread *CFS_func(void){
#warning TODO
    uThread *thread = (uThread *) malloc(sizeof(uThread));
    return thread;
}

uThread *next_to_schedule(void){
    if(policy == RR)
        return RR_func();
    //else -> CFS
    return CFS_func();
}

int scheduler_add_thread(uThread *thread){//adds thread to the appropriate data structure
    if(policy){//CFS
#warning TODO CFS
        return 0;
    }
    else{//RR
        queue_element *element = (queue_element *) malloc(sizeof(queue_element));
        if(element == NULL) //if malloc failed
            return -1; //errno is already set by malloc
        //init element
        element->element = thread;
        element->next = NULL;
        //add element to the queue at the end
        pthread_mutex_lock(&mutex); //other threads are not able to acces protects ressources utile we release the lock
        if(queue->last != NULL)
            queue->last->next = element;
        queue->last = element;
        pthread_mutex_unlock(&mutex);
        return 0;
    }
}
