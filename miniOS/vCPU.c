//
//  vCPU.c
//  miniOS
//
//  Created on 15/04/2022.
//
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "vCPU.h"


vCPU* create_vCPU(void){
    pthread_t *thread = malloc(sizeof(thread));
    
    vCPU *cpu = (vCPU*) malloc(sizeof(vCPU));
    
    pthread_create(thread, NULL, idle, NULL); //crée un thread (vCPU) et le suspend jusqu'à reception d'un signal
    cpu->pthread = thread;
    
    return cpu;
}
