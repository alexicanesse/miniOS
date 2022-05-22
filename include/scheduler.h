//
//  scheduler.h
//  miniOS
//
//  Created on 15/04/2022.
//

#define _XOPEN_SOURCE

#ifndef scheduler_h
#define scheduler_h

#include <stdio.h>
#include <ucontext.h>

#include "vCPU.h"
#include "miniOS.h"

struct scheduler_type{
    int quantum;
    uThread *(*func)(void);
};
typedef struct scheduler_type scheduler_type;


void config_scheduler(int quantum, enum scheduler_policies scheduler_policy);


int scheduler_add_thread(uThread *thread); //return -1 if fails and sets errno. Otherwise, return 0

uThread *RR_func(void); //Round-robin scheduling
uThread *CFS_func(void); //Completely Fair Scheduler
uThread *next_to_schedule(uThread * thread);






#endif /* scheduler_h */
