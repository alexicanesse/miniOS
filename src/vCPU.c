//
//  vCPU.c
//  miniOS
//
//  Created on 15/04/2022.
//
//requiered by ucontext
#define _XOPEN_SOURCE

//tells gcc that we do not want to know about deprecation.
//without it, some systems complains because contexts are deprecated
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#include <stdlib.h>
#include <pthread.h>
#include <ucontext.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <signal.h>

#include "vCPU.h"
#include "scheduler.h"
#include "miniOS.h"

#ifdef WITH_OWN_HMM
extern void* malloc(size_t size);
extern void* realloc(void* ptr, size_t size);
extern void free(void* ptr);
#endif

typedef void (* ucfunc_t)(void);

__thread int must_end = 0; /* this variable is used to be able to wait for the quantum to end before deleting a vCPU */
ucontext_t *uThread_cleaner(uThread *uthread);

vCPU *vCPUs = NULL; /* list of all running vCPUs */
uThread *uThreads = NULL; /* list of all running uThreads */
extern scheduler_type scheduler;

/* __thread is a wrapper to make a globale variable thread specific */
__thread ucontext_t *current_context = NULL; 
__thread uThread *current_uThread = NULL;

int create_vCPU(int nbr_vCPU){
    while(nbr_vCPU--){ /* each loop creats one vCPU */
        pthread_t *thread = (pthread_t*) malloc(sizeof(pthread_t));
        if(thread == NULL)/* if malloc failed */
            return -1; /* malloc already sets errno correctly */
        
        vCPU *cpu = (vCPU*) malloc(sizeof(vCPU));
        if(cpu == NULL){ /* if malloc failed */
            free(thread);
            return -1; /* malloc already sets errno correctly */
        }
        
        /* create a thread (vCPU) and suspend it until a signal wakes it up */
        if(pthread_create(thread, NULL, init, NULL)){ //if return 0, it worked
            free(thread);
            free(cpu);
            return -1; /* pthread_create already sets errno correctly */
        }
            
        cpu->pthread = thread;
        
        /* add the vCPU to the list of vCPUs */
        cpu->next = vCPUs;
        vCPUs = cpu;
    }
    
    return 0;
}


/*
 * removes the vCPU from the list
 * alerts the vCPU that it needs to be terminated
 * join with the thread
 */
int destruct_vCPU(int nbr_vCPU){
    while(nbr_vCPU--){
        if(vCPUs == NULL) /* There isn't any vCPU left to delete */
            return -1; /* we were not able to delete enough vCPUs */
        

        vCPU *cpu_buffer = vCPUs;
        vCPUs = vCPUs->next; /* the first vCPU is removed from the list */
        
        /* the thread is canceled */
        pthread_kill(*cpu_buffer->pthread, SIGUSR2);
        
        /* free the memory */
        free(cpu_buffer->pthread);
        free(cpu_buffer);
    }
    return 0;
}

int create_uThread(void (*func)(void), int argc, const char * argv[]){
    uThread *thread = (uThread *) malloc(sizeof(uThread));
    if(thread == NULL) /* if malloc failed */
        return -1; /* malloc already sets errno correctly */
    
    /* get stack size of the os */
    struct rlimit lim;
    getrlimit(RLIMIT_STACK, &lim);
    thread->stack_size = lim.rlim_cur;
    
    /* init the context */
    ucontext_t *context = (ucontext_t *) malloc(sizeof(ucontext_t));
    thread->context = context;
    if(context == NULL){ /* if malloc failed */
        free(thread);
        return -1; /* malloc already sets errno correctly */
    }
    
    
    /* sets the stack of the uThread */
    char *stack = (char*) malloc(thread->stack_size);
    thread->stack = stack;
    if(stack == NULL){ /* if malloc failed */
        free(thread);
        free(context);
        return -1; /* malloc already sets errno correctly */
    }
    
    /* set up the context */
    getcontext(context);
    context->uc_stack.ss_sp = stack;
    context->uc_stack.ss_size = thread->stack_size;
    context->uc_link = uThread_cleaner(thread); /* the context to clean the uThread when it finishes */
    makecontext(context, func, argc, argv);
    if(errno != 0){ /* makecontext failed */
        free(thread);
        free(context);
        free(stack);
        fprintf(stderr, "%s", strerror(errno)); /* prints the error message */
        return -1;
    }
    
    thread->running = 0;
    
    /* we schedule it */
    if(scheduler_add_thread(thread) != 0)
        return -1;

    return 0;
}

void destruct_current_uThread(uThread* thread){
    if(thread == NULL)
        return; /* this is a protection */

    /* we set the current_thread var to NULL so it won't be scheduled again */
    current_context = NULL;
    current_uThread = NULL;

    /* we delete the thread from the thread list */
    uThread *thread_it = uThreads;
    uThread *thread_buff = NULL;
    while(thread_it != NULL){
        if(thread_it == thread){
            if(thread_buff == NULL)
                uThreads = thread->next;
            else
                thread_buff->next = thread->next;

            free(thread->stack);
            free(thread->context);
            free(thread);
            yield();
        }
        thread_buff = thread_it;
        thread_it = thread_it->next;
    }
    yield();
}

/*
 * send a sigusr1 to itself as if it was time to schedule an other thread
 */
int yield(void){
    switch_process(SIGUSR1, NULL, NULL);
    return 0;
}


void *init(void* param){ /* suspends until a signal is received */
    static struct sigaction _sigact;
    memset(&_sigact, 0, sizeof(_sigact));
    _sigact.sa_sigaction = switch_process;
    _sigact.sa_flags = SA_SIGINFO;
    sigfillset(&_sigact.sa_mask);
    
    sigaction(SIGUSR1, &_sigact, NULL);
    
    static struct sigaction _sigact2;
    memset(&_sigact2, 0, sizeof(_sigact2));
    _sigact2.sa_sigaction = switch_process;
    _sigact2.sa_flags = SA_SIGINFO;
    sigfillset(&_sigact2.sa_mask);
    
    sigaction(SIGUSR2, &_sigact2, NULL);

    idle();
    return NULL;
}

void idle(void){
    sigset_t sigmask;
    sigemptyset(&sigmask);
    
    while(1){
        sigsuspend(&sigmask); /* there is no need to waste CPU cycles while we wait */
    }
}

void switch_process(int signum, siginfo_t *info, void *ptr){
    /* the thread is no longer running */
      if(current_uThread != NULL)
          current_uThread->running = 0;
      
      if(must_end == 1){ /* the vCPU is going to be canceled */
          scheduler_add_thread(current_uThread);
          pthread_cancel(pthread_self());
          return; /* just in case */
      }

      /* get the next thread to schedule */
      uThread *thread = next_to_schedule(current_uThread);
      current_uThread = thread;
      
      if(thread == NULL)
          idle(); /* if there is nothing to schedule, put the thread in idle */
      else{
          thread->running = 1;
          ucontext_t *past_context = current_context;
          current_context = thread->context;
          if(past_context == NULL)
              setcontext(current_context);
          else
              swapcontext(past_context, current_context);
      }
}

ucontext_t *uThread_cleaner(uThread *uthread){
    /* get stack size of the os */
    struct rlimit lim;
    getrlimit(RLIMIT_STACK, &lim);
    long int stack_size = lim.rlim_cur;
    
    ucontext_t *context = (ucontext_t *) malloc(sizeof(ucontext_t));
    if(context == NULL) /* if malloc failed */
        return NULL; /* malloc already sets errno correctly */
    
    char *stack = (char*) malloc(stack_size);
    if(stack == NULL){ /* if malloc failed */
        free(context);
        return NULL; /* malloc already sets errno correctly */
    }
    
    /* set up the context */
    getcontext(context);
    context->uc_stack.ss_sp = stack;
    context->uc_stack.ss_size = stack_size;
    makecontext(context, (ucfunc_t)destruct_current_uThread, 1, uthread);
    if(errno != 0){ /* makecontext failed */
        free(context);
        free(stack);
        fprintf(stderr, "%s", strerror(errno)); /* prints the error message */
        return NULL;
    }

    return context;
}


void end_vCPU(int i, struct __siginfo * info, void * ptr){
    must_end = 1;
}
