//
//  miniOS_private.h
//  miniOS
//
//  Created on 16/04/2022.
//


//this file contains the definitions that are not meant to be used by the user

#ifndef miniOS_private_h
#define miniOS_private_h

//vcpu
struct vCPU{
    pthread_t *pthread;
    struct vCPU *next; //used to make lists
};
typedef struct vCPU vCPU;

void *init(void* param);
void idle(void);
void switch_process(int signum, siginfo_t *info, void *ptr);






//scheduler
int scheduler_add_thread(uThread *thread); //return -1 if fails and sets errno. Otherwise, return 0

uThread *RR_func(void); //Round-robin scheduling
uThread *CFS_func(void); //Completely Fair Scheduler
uThread *next_to_schedule(void);


#endif /* miniOS_private_h */
