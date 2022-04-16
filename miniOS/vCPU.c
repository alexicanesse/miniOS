//
//  vCPU.c
//  miniOS
//
//  Created on 15/04/2022.
//
//requiered by ucontext
#define _XOPEN_SOURCE

#include <stdlib.h>
#include <pthread.h>
#include <ucontext.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include<stdio.h>

#include "vCPU.h"

//here because the user should not have access to it
struct vCPU{
    pthread_t *pthread;
    struct vCPU *next; //used to make lists
};
typedef struct vCPU vCPU;

void *idle(void* param);
//




vCPU *vCPUs = NULL; //list of all running vCPUs
uThread *uThreads = NULL; //list of all running uThreads

int create_vCPU(int nbr_vCPU){
    while(nbr_vCPU--){
        pthread_t *thread = (pthread_t*) malloc(sizeof(thread));
        vCPU *cpu = (vCPU*) malloc(sizeof(vCPU));
    
        if(thread == NULL || cpu == NULL)//if malloc failed
            return -1; //malloc already sets errno correctly
        
        //create a thread (vCPU) and suspend it until a signal wakes it up
        if(pthread_create(thread, NULL, idle, NULL)) //if return 0, it worked
            return -1; //pthread_create already sets errno correctly
            
        cpu->pthread = thread;
        
        //add the vCPU to the list of vCPUs
        cpu->next = vCPUs;
        vCPUs = cpu;
    }
    
    return 0;
}

int destruct_vCPU(int nbr_vCPU){
    while(nbr_vCPU--){
        if(vCPUs == NULL) //There isn't any vCPU left to delete
            return -1; //we were not able to delete enough vCPUs
        
        vCPU *cpu_buffer = vCPUs;
        vCPUs = vCPUs->next; //the first vCPU is removed from the list
        
        //free the memory
        free(cpu_buffer->pthread);
        free(cpu_buffer);
    }
    return 0;
}

int create_uThread(void (*func)(void), int argc, const char * argv[]){
    uThread *thread = (uThread *) malloc(sizeof(uThread*));
    if(thread == NULL) //if malloc failed
        return -1; //malloc already sets errno correctly
    
    //get stack size of the os
    struct rlimit lim;
    getrlimit(RLIMIT_STACK, &lim);
    thread->stack_size = lim.rlim_cur;
    
    ucontext_t *context = (ucontext_t *) malloc(sizeof(ucontext_t));
    thread->context = context;
    if(context == NULL) //if malloc failed
        return -1; //malloc already sets errno correctly
    
    
    
    char *stack = (char*) malloc(thread->stack_size);
    thread->stack = stack;
    if(stack == NULL) //if malloc failed
        return -1; //malloc already sets errno correctly
    
    //set up the context
    errno = 0; //we must check errno because makecontext does not return a value
    getcontext(context);
    context->uc_stack.ss_sp = stack;
    context->uc_stack.ss_size = thread->stack_size;
#warning I have some doubts about the arguments
    makecontext(context, func, argc, argv);
    if(errno != 0){ //makecontext failed
        fprintf(stderr, "%s", strerror(errno)); //prints the error message
        return -1;
    }
    
    //add the thread to the list of threads
    thread->next = uThreads;
    uThreads = thread;
    
    return 0;
}

int yield(uThread* thread){
#warning it still need to be implemented: it will send the same signal as the alarm to the scheduler
    return 0;
}

#warning il faut limite le sigsuspend à un signal user qui servira à ca (plus safe)
void *idle(void* param){ //suspends until a signal is received
    sigset_t sigmask;
    sigemptyset(&sigmask);
    
    sigsuspend(&sigmask);
    return NULL;
}
