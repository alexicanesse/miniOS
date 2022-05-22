//
//  miniOS.c
//  miniOS
//
//  Created on 14/04/2022.
//

#include <sys/time.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>

#include "miniOS.h"
#include "vCPU.h"
#include "scheduler.h"

#ifdef WITH_OWN_HMM
void* malloc(size_t size){
    return cls_malloc(size);
}

void* realloc(void* ptr, size_t size){
    return cls_realloc(ptr, size);
}

void free(void* ptr){
    cls_free(ptr);
}
#endif

extern scheduler_type scheduler;
extern vCPU *vCPUs;

/*
 * this handler is declared here because the user should not be able to access it
 */
void handle_alarm(int signum, siginfo_t *info, void *ptr);

//this function create a timer well set up for the current quantum. Each time the alarm rings, we send a SIGUSR1 to each thread. It tells them to switch to the next uThread.
void run(void){
    //set timer
    struct itimerval it;
    it.it_interval.tv_sec = scheduler.quantum;
    it.it_interval.tv_usec = 0;
    it.it_value = it.it_interval;
    if (setitimer(ITIMER_REAL, &it, NULL) )
        perror("Problème de setitiimer");

    //set handler
    static struct sigaction _sigact;
    memset(&_sigact, 0, sizeof(_sigact));
    _sigact.sa_sigaction = handle_alarm;
    _sigact.sa_flags = SA_SIGINFO | SA_RESTART;
    sigfillset(&_sigact.sa_mask);
    
    sigaction(SIGALRM, &_sigact, NULL);
}

void handle_alarm(int signum, siginfo_t *info, void *ptr){
    vCPU *cpu = vCPUs;
    while(cpu != NULL){
        pthread_kill(*cpu->pthread, SIGUSR1); //It tells them to switch to the next uThread.
        cpu = cpu->next;
    }
}
