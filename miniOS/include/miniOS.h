//
//  miniOS.h
//  miniOS
//
//  Created on 14/04/2022.
//

//This is the only header the user will import

#ifndef miniOS_h
#define miniOS_h

#include "vCPU.h"
#include "scheduler.h"
#include "memory.h"

void run(void);



/*
 * scheduler
 */
enum scheduler_policies;
extern void config_scheduler(int quantum, enum scheduler_policies scheduler_policy);


/*
 * vCPU
 */
struct uThread;
typedef struct uThread uThread;

//create or destruct nbr_vCPU vCPUs
extern int create_vCPU(int nbr_vCPU); //returns 0 unless it fails. errno is set if it fails
extern int destruct_vCPU(int nbr_vCPU); //returns 0 unless it fails. errno is set if it fails

//the stack_size used is the same as the one of the os.
extern int create_uThread(void (*func)(void), int argc, const char * argv[]); //returns 0 unless it fails. errno is set if it fails
extern void destruct_current_uThread(uThread* thread); //returns 0 unless it fails. errno is set if it fails
extern int yield(void);



/*
 * Heap memory manager
 */
extern void *hm_malloc(long int size);
extern void *hm_realloc(void* ptr, long int size);
extern void hm_free(void *ptr);

#endif /* miniOS_h */
