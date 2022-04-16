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


vCPU *vCPUs = NULL; //list of all running vCPUs


int create_vCPU(int nbr_vCPU){
    while(nbr_vCPU--){
        pthread_t *thread = (pthread_t*) malloc(sizeof(thread));
        vCPU *cpu = (vCPU*) malloc(sizeof(vCPU));
    
        if(thread == NULL || cpu == NULL)//if malloc failed
            return -1; //malloc already sets errno correctly
        
        //create a thread (vCPU) and suspend it until a signal wakes it up
        if(pthread_create(thread, NULL, idle, NULL)) //if return 0, it worked
            return -1; //pthread_create already sets errno correctly
            
        cpu->pthread = thread;
        
        //add the vCPU to the list of vCPUs
        cpu->next = vCPUs;
        vCPUs = cpu;
    }
    
    return 0;
}

int destruct_vCPU(int nbr_vCPU){
    while(nbr_vCPU--){
        if(vCPUs == NULL) //There isn't any vCPU left to delete
            return -1; //we were not able to delete enough vCPUs
        
        vCPU *cpu_buffer = vCPUs;
        vCPUs = vCPUs->next; //the first vCPU is removed from the list
        
        //free the memory
        free(cpu_buffer->pthread);
        free(cpu_buffer);
    }
    return 0;
}
