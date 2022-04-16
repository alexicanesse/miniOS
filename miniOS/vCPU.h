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


//void *idle(void* param);c





#endif /* vCPU_h */
