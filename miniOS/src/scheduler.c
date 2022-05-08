//
//  scheduler.c
//  miniOS
//
//  Created on 15/04/2022.
//
#include <stdlib.h>

#include "scheduler.h"
#include "vCPU.h"
#include "uThread_queue.h"

uThread_queue *queue;

enum scheduler_policies policy = RR;
scheduler_type scheduler;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; //lock use to make some ressources thread-safe

void config_scheduler(int quantum, enum scheduler_policies scheduler_policy){ //set the schedule up
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
    if(queue->first == NULL || queue->first->element->running == 1) //if there is nothing to schedule,
        return NULL; //the thread will idle
    
    uThread *thread = dequeue(); //get the thread at the end of the queue and remove it from the queue
//    enqueue(thread); //it needs to be scheduled back otherwise it would not be able to ever finish
    
    return thread;
}

uThread *CFS_func(void){
#warning TODO
    uThread *thread = (uThread *) malloc(sizeof(uThread));
    return thread;
}

uThread *next_to_schedule(uThread *thread){ //asks the function associated with the current policy what to schedule next
    if(policy == RR){
        if(thread != NULL){
            thread->running = 0;
            enqueue(thread);
        }
        return RR_func();
    }
    //else -> CFS
    return CFS_func();
}

int scheduler_add_thread(uThread *thread){//adds thread to the appropriate data structure
    if(policy){//CFS
#warning TODO CFS
        return 0;
    }
    else //RR
        return enqueue(thread);
}


