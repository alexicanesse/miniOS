//
//  scheduler.c
//  miniOS
//
//  Created on 15/04/2022.
//
#include <stdlib.h>
#include <time.h>

#include "scheduler.h"
#include "vCPU.h"
#include "uThread_queue.h"
#include "uThread_tree.h"
#include "miniOS.h"

#ifdef WITH_OWN_HMM
extern void* malloc(size_t size);
extern void* realloc(void* ptr, size_t size);
extern void free(void* ptr);
#endif

uThread_queue *queue;
uThread_tree *tree;

enum scheduler_policies policy = RR;
scheduler_type scheduler;
__thread struct timespec timestamp;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; /* lock used to make some ressources thread-safe */

void config_scheduler(int quantum, enum scheduler_policies scheduler_policy) { /* set the schedule up */
    scheduler.quantum = quantum;

    /* configure the scheduler accordingly to the policy choosen */
    if (scheduler_policy == CFS){ /*CFS */
        policy = CFS;
        scheduler.func = CFS_func;
        tree = NULL;
    }
    else{ /* RR */
        scheduler.func = RR_func;
        policy = RR;
        if (queue == NULL)
            queue = empty_queue();
    }
}

uThread *RR_func(void) { /* we update the queue return the next uThread to schedule */
    if (queue->first == NULL || queue->first->element->running == 1) /*if there is nothing to schedule, */
        return NULL; /* the thread will idle */

    uThread *thread = dequeue(); /* get the thread at the end of the queue and remove it from the queue */
    return thread;
}

uThread *CFS_func(void) {
    if (tree == NULL) /* if the tree is empty, there is nothing to schedule */
        return NULL;

    pthread_mutex_lock(&mutex);
    uThread *thread = tree->leftmost->thread; /* get the leftmost thread */
    tree = remove_node(tree->leftmost, tree); /* then remove it from the tree */
    pthread_mutex_unlock(&mutex);
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &timestamp);
    return thread;
}

/* asks the function associated with the current policy what to schedule next */
uThread *next_to_schedule(uThread *thread) {
    if(policy == RR){ /* RR */
        if (thread != NULL) {
            thread->running = 0;
            enqueue(thread);
        }
        return RR_func();
    } else{/* CFS */
        if (thread != NULL) {
            struct timespec ts_out;
            clock_gettime(CLOCK_THREAD_CPUTIME_ID, &ts_out);
            thread->running = 0;
            thread->vTime += ts_out.tv_nsec - timestamp.tv_nsec;
            pthread_mutex_lock(&mutex);
            tree = insert(thread, tree);
            pthread_mutex_unlock(&mutex);
        }
        return CFS_func();
    }
}

int scheduler_add_thread(uThread *thread) {/* adds thread to the appropriate data structure */
    if (policy){ /* CFS */
        /* New threads are inserted with the minimal virtual time (just as if they were there from the beginning) */
        if (tree == NULL)
            thread->vTime = 0;
        else
            thread->vTime = tree->leftmost->thread->vTime;
        pthread_mutex_lock(&mutex);
        tree = insert(thread, tree);
        pthread_mutex_unlock(&mutex);
        return 0;
    }
    else /* RR */
        return enqueue(thread);
}


