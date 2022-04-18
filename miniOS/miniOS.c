//
//  miniOS.c
//  miniOS
//
//  Created on 14/04/2022.
//

#include <sys/time.h>
#include <signal.h>

#include "miniOS.h"
#include "miniOS_private.h"


extern scheduler_type scheduler;
extern vCPU *vCPUs;
void run(void){
    //set timer
    struct itimerval it;
    it.it_interval.tv_sec = scheduler.quantum;
    it.it_interval.tv_usec = 0;
    it.it_value = it.it_interval;
    if (setitimer(ITIMER_REAL, &it, NULL) )
        perror("Problème de setitiimer");

    struct sigaction sa;
    sa.sa_handler = &handle_alarm;
    sigfillset(&sa.sa_mask);
    sigaction(SIGALRM, &sa, NULL);
}

void handle_alarm(int signal) { // on dit à tous les vCPU de lancer un nouveau thread
    vCPU *cpu = vCPUs;
    while(cpu != NULL){
        pthread_kill(cpu->pthread, SIGUSR1);
        cpu = cpu->next;
    }
}
