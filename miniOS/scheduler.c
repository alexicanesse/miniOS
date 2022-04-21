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
#include "uThread_queue.h"

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
//        pthread_mutex_lock(&mutex); //other threads are not able to acces protects ressources utile we release the lock
//        create_uThread(idle, 0, NULL); //we schedule idle() (it does a sigsuspend)
//        uThread *thread = pop_last();
//        pthread_mutex_unlock(&mutex);
//        return thread;
        return NULL;
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


