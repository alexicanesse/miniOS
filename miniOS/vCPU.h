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

struct vCPU{ //je definie une strcut au cas où on veut y ajouter des choses
    pthread_t *pthread;
};
typedef struct vCPU vCPU;

struct uThread{ //a enrichir suivant le besoin
    int uPID; //sert à identifier le thread
};
typedef struct uThread uThread;

#warning selon l'exemple tu tableau il faut ajouter un argument mais je ne sais pas lequel
vCPU* create_vCPU(void);
int destruct_vCPU(vCPU* cpu); //le int sert aux messages d'erreur

#warning arguments à definir
uThread* create_uThread(void);
int destruct_uThread(uThread* thread);
int yield(uThread* thread);

#warning il faut limite le sigsuspend à un signal user qui servira à ca (plus safe)
void *idle(void* param){ //suspend jusqu'à ce qu'un signal soit recu.
    sigset_t sigmask;
    sigemptyset(&sigmask);
    
    sigsuspend(&sigmask);
    return NULL;
}





#endif /* vCPU_h */
