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

typedef uint8_t byte;

struct mem_block_s{
    char *ptr;
    long int size;
    struct mem_block_s *next; //used to make a linked list
};
typedef struct mem_block_s mem_block_t;


struct pair_s{
    void * first;
    void *second;
};
typedef struct pair_s pair_t;

struct used_s{
    void *ptr; /* NULL indicates unused slot in list */
    size_t size; 
    struct used_s *next;
};
typedef struct used_s used_t;

struct mem_class{
    mem_block_t *free_list; /* FIFO */
    used_t *used; /* grows from bottom of the same address space than the free list */
    int free_list_size; /* number of element left in the free list */
    void *first_page_addr;
    int last_used_page;
    pthread_mutex_t mutex; /* each class has its own mutex to limit the perfomance overhead */
    pair_t range;
};
typedef struct mem_class mem_class_t;



int initclsarr(void);
int initclass(int cls_index);
int addfreespace(int cls_index);
int clsindex(size_t size);
int ptrtoclsindex(void *ptr);
void insert_in_last_used_list(int cls_index, void* ptr, size_t size);
int delete_in_last_used_list(int cls_index, void* ptr);
size_t get_size(void* ptr, int cls_index);
byte ptr_to_cannary(void *ptr);
int block_alloced(void *ptr);

void* cls_malloc(size_t size);
void* cls_realloc(void* ptr, size_t size);
void cls_free(void* ptr);


#endif /* memory_h */
