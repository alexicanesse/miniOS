//
//  vCPU.h
//  miniOS
//
//  Created on 15/04/2022.
//

#ifndef vCPU_h
#define vCPU_h
//requiered by ucontext
#define _XOPEN_SOURCE

#include <pthread.h>
#include <unistd.h>
#include <signal.h>



struct uThread{
    ucontext_t *context;
    char *stack;
    unsigned long long stack_size;
    struct uThread *next; //used to make lists
};
typedef struct uThread uThread;

//create or destruct nbr_vCPU vCPUs 
int create_vCPU(int nbr_vCPU); //returns 0 unless it fails. errno is set if it fails
int destruct_vCPU(int nbr_vCPU); //returns 0 unless it fails. errno is set if it fails

//the stack_size used is the same as the one of the os.
int create_uThread(void (*func)(void), int argc, const char * argv[]); //returns 0 unless it fails. errno is set if it fails
int destruct_uThread(uThread* thread); //returns 0 unless it fails. errno is set if it fails
int yield(uThread* thread);





#endif /* vCPU_h */
