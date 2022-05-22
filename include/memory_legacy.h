//
//  memory_legacy.h
//  miniOS
//
//  Created on 14/05/2022.
//

#ifndef memory_legacy_h
#define memory_legacy_h

#include <stdio.h>

struct mem_block{
    char *ptr;
    long int size;
    struct mem_block *next; //used to make a linked list
    int is_used;
    int is_brk;
    int is_secure;
    struct mem_block *prev; //used to make a double linked list
};
typedef struct mem_block mem_block;




void insert_block(mem_block *block);
void fusion_with_next(mem_block *block);
mem_block *init_mem_block(void);

void *hm_malloc(size_t size);
void *pmalloc(size_t size);
void *hm_realloc(void* ptr, long int size);
void hm_free(void *ptr);


#endif /* memory_legacy_h */
