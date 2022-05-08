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

struct scheduler_type{
    int quantum;
    uThread *(*func)(void);
};
typedef struct scheduler_type scheduler_type;

enum scheduler_policies { RR, CFS };
void config_scheduler(int quantum, enum scheduler_policies scheduler_policy);


 






#endif /* scheduler_h */
