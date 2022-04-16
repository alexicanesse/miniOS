//
//  vCPU.h
//  miniOS
//
//  Created on 15/04/2022.
//

#ifndef vCPU_h
#define vCPU_h

#include <pthread.h>
#include <unistd.h>
#include <signal.h>

struct vCPU{
    pthread_t *pthread;
    struct vCPU *next; //used to make lists
};
typedef struct vCPU vCPU;

struct uThread{
    int uPID; //thread id
};
typedef struct uThread uThread;

//create or destruct nbr_vCPU vCPUs 
int create_vCPU(int nbr_vCPU); //returns 0 unless it fails. errno is set if it fails
int destruct_vCPU(int nbr_vCPU); //returns 0 unless it fails. errno is set if it fails

#warning arguments à definir
uThread* create_uThread(void);
int destruct_uThread(uThread* thread);
int yield(uThread* thread);

#warning il faut limite le sigsuspend à un signal user qui servira à ca (plus safe)
void *idle(void* param){ //suspends until a signal is received
    sigset_t sigmask;
    sigemptyset(&sigmask);
    
    sigsuspend(&sigmask);
    return NULL;
}





#endif /* vCPU_h */
