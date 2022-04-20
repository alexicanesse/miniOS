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


uThread_queue* empty_queue(void){
    uThread_queue *queue = (uThread_queue *) malloc(sizeof(uThread_queue));
    queue->first = NULL;
    queue->last = NULL;
    return queue;
}
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
        if(queue == NULL)
            queue = empty_queue();
    }
}

uThread *RR_func(void){ //we update the queue and switch to the next uThread
    while(queue->first == NULL || queue->first->element->running == 1){ //if there is nothing to schedule, schedule idle
        pthread_mutex_lock(&mutex); //other threads are not able to acces protects ressources utile we release the lock
        create_uThread(idle, 0, NULL);
        uThread *thread = pop_last();
        pthread_mutex_unlock(&mutex);
        return thread;
    }
    uThread *thread = dequeue();
    enqueue(thread);
    
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
        return enqueue(thread);
    }
}



int enqueue(uThread *thread){
    queue_element *element = (queue_element *) malloc(sizeof(queue_element));
    if(element == NULL) //if malloc failed
        return -1; //errno is already set by malloc
    //init element
    element->element = thread;
    element->next = NULL;
    //add element to the queue at the end
    pthread_mutex_lock(&mutex); //other threads are not able to acces protects ressources utile we release the lock
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
    uThread *thread = queue->first->element; //next thread

    pthread_mutex_lock(&mutex); //other threads are not able to acces protects ressources utile we release the lock
    if(queue->first == queue->last){//there is only one node
        queue->first = NULL;
        queue->last = NULL;
    }
    else
        queue->first = queue->first->next;
    //we realease the memory
//    free(buffer);
    pthread_mutex_unlock(&mutex);

    
    return thread;
}

uThread *pop_last(void){ //not locked! be carefull
    uThread *thread = queue->last->element;
    if(queue->first == queue->last){//there is only one node
        queue->first = NULL;
        queue->last = NULL;
    }
    else{
        uThread_queue *q_buff= queue;
        while(q_buff->first->next != queue->last)
            q_buff->first = q_buff->first->next;
        queue->last = q_buff->first;
    }
    return thread;
}
