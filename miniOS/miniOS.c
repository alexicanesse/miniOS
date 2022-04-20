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
#include "miniOS_private.h"


extern scheduler_type scheduler;
extern vCPU *vCPUs;

void handle_alarm(int signum, siginfo_t *info, void *ptr);

void run(void){
    //set timer
    struct itimerval it;
    it.it_interval.tv_sec = scheduler.quantum;
    it.it_interval.tv_usec = 0;
    it.it_value = it.it_interval;
    if (setitimer(ITIMER_REAL, &it, NULL) )
        perror("Problème de setitiimer");

    static struct sigaction _sigact;
    memset(&_sigact, 0, sizeof(_sigact));
    _sigact.sa_sigaction = handle_alarm;
    _sigact.sa_flags = SA_SIGINFO;

    sigaction(SIGALRM, &_sigact, NULL);
    idle();
}

void handle_alarm(int signum, siginfo_t *info, void *ptr){
    printf("\n");
    vCPU *cpu = vCPUs;
    while(cpu != NULL){
        pthread_kill(*cpu->pthread, SIGUSR1);
        cpu = cpu->next;
    }
}
