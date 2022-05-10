//
//  memory.h
//  miniOS
//
//  Created on 15/04/2022.
//

#ifndef memory_h
#define memory_h

#include <stdio.h>



struct mem_block{
    char *ptr;
    long int size;
    int is_used;
    int is_brk;
    int is_secure;
    struct mem_block *next; //used to make a linked list
    struct mem_block *prev; //used to make a double linked list
};
typedef struct mem_block mem_block;


void insert_block(mem_block *block);


/*
 * This function is the core malloc function
 * hm_malloc just call it with secure = 0 and pmalloc with secure = 1
 */
void *hm_malloc_func(long int size, int secure);

void *hm_malloc(long int size);
void *pmalloc(long int size);
void *hm_realloc(void* ptr, long int size);
void hm_free(void *ptr);


#endif /* memory_h */
