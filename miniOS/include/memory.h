//
//  memory.h
//  miniOS
//
//  Created on 15/04/2022.
//

#ifndef memory_h
#define memory_h

#include <stdio.h>

//https://vtechworks.lib.vt.edu/bitstream/handle/10919/96291/Liu_B_T_2020.pdf

struct mem_block{
    char *ptr;
    long int size;
    struct mem_block *next; //used to make a linked list
#warning TODO remove this
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



struct mem_class{
    mem_block *free_list; /* FIFO */
    int free_list_size; /* number of element left in the free list */
    void *first_page_addr;
    int last_used_page;
    pthread_mutex_t mutex; /* each class has its own mutex to limit the perfomance overhead */
};
typedef struct mem_class mem_class_t;

int initclsarr(void);
int initclass(int cls_index);
int addfreespace(int cls_index);
int clsindex(size_t size);



#endif /* memory_h */
