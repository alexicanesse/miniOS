//
//  memory.c
//  miniOS
//
//  Created on 15/04/2022.
//

/*
 * recommanded default value
 * must be a power of two to be fully respected
 */
#define M_MMAP_THRESHOLD 128*1024


//tells gcc that we do not want to know about deprecation.
//without it, some systems complains because contexts are deprecated
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h> //requiered for mmap
#include <sys/mman.h>
#include <string.h>
#include <math.h>
#include <errno.h>

#include "memory.h"

#define MAX_CLASS_INDEX 12
#define MAX_PAGE_CLASS 1000000000
#define MIN_NUMBER_OF_BLOCK_TO_ALLOC 10
#define NUMBER_OF_PAGES_FOR_FREE_LIST 300

/*
 * the classes 0->MAX_CLASS_INDEX-1 will be used for allocation of size >= 2^i and < 2^(i+1)
 * The last class is used for all other allocations
 */
mem_class_t *class_array = NULL;
pthread_mutex_t mutex_clsarr = PTHREAD_MUTEX_INITIALIZER;
long int  seed = 0;

/*
 * mutex over all function because if another thread tries to init the class, it needs to wait for it
 * to be init
 */
int initclsarr(void){
    pthread_mutex_lock(&mutex_clsarr);
    if(class_array != NULL){
        pthread_mutex_unlock(&mutex_clsarr);
        return 0;
    }

    seed = random();
    size_t PAGESIZE = sysconf(_SC_PAGE_SIZE);
    
    void* address = mmap(0x0, /* If addr is zero and MAP_FIXED is not specified, then an
                         address will be selected by the system so as not to overlap any existing
                         mappings in the address space. */
                    PAGESIZE /* the page for the data*/
                   + PAGESIZE  /* the page guard before the data */
                   + PAGESIZE, /* the page guard after the data */
         PROT_READ | PROT_WRITE, /* we can do whatever we want with it */
         MAP_SHARED | MAP_ANONYMOUS,
         0,
         0);
    
    if(address == MAP_FAILED){
        pthread_mutex_unlock(&mutex_clsarr);
        return -1;
    }

    /* set guard pages */
    if(mprotect(address, PAGESIZE, PROT_NONE) || mprotect(address + 2*PAGESIZE, PAGESIZE, PROT_NONE)){
        munmap(address, 3*PAGESIZE); /* sets errno if it fails */
        pthread_mutex_unlock(&mutex_clsarr);
        return -1;
    }

    class_array = address + PAGESIZE; /* + PAGESIZE because the first page is a guard page */
    for(int i = 0; i <= MAX_CLASS_INDEX; i++){ /* init classes */
        class_array[i].first_page_addr = NULL;
        class_array[i].free_list = NULL;
        class_array[i].used = NULL;
        class_array[i].mutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
        class_array[i].last_used_page = 0;
        class_array[i].free_list_size = 0;
        class_array[i].range.first = 0;
        class_array[i].range.second = 0;
    }
    pthread_mutex_unlock(&mutex_clsarr);
    return 0;
}

/*
 * mutex over all function because if another thread tries to init the class, it needs to wait for it
 * to be init
 */
int initclass(int cls_index){
    pthread_mutex_lock(&class_array[cls_index].mutex);
    if(class_array[cls_index].first_page_addr != NULL)
        return 0; /* already init */
    
    size_t PAGESIZE = sysconf(_SC_PAGE_SIZE);
    
    void *addr = mmap(
         0x0, /* If addr is zero and MAP_FIXED is not specified, then an
               address will be selected by the system so as not to overlap any existing
               mappings in the address space. */
         PAGESIZE * MAX_PAGE_CLASS,
         PROT_NONE,
         MAP_SHARED | MAP_ANONYMOUS,
         0,
         0);
    
    if(addr == MAP_FAILED){
        pthread_mutex_unlock(&class_array[cls_index].mutex);
        return -1;
    }
    
    class_array[cls_index].first_page_addr = addr;
    class_array[cls_index].range.first = addr;
    class_array[cls_index].range.second = addr + PAGESIZE * MAX_PAGE_CLASS;
    
    /*
     * create the free list and secure it with guard pages.
     */
    void* free_list = mmap(0x0, /* If addr is zero and MAP_FIXED is not specified, then an
                         address will be selected by the system so as not to overlap any existing
                         mappings in the address space. */
                    NUMBER_OF_PAGES_FOR_FREE_LIST * PAGESIZE, /* this includes the two page guards */
         PROT_READ | PROT_WRITE, /* we can do whatever we want with it */
         MAP_SHARED | MAP_ANONYMOUS,
         0,
         0);
    
    if(free_list == MAP_FAILED){
        pthread_mutex_unlock(&class_array[cls_index].mutex);
        return -1;
    }
    
    /* set guard pages */
    if(mprotect(free_list, PAGESIZE, PROT_NONE) || mprotect(free_list + (NUMBER_OF_PAGES_FOR_FREE_LIST - 1)*PAGESIZE, PAGESIZE, PROT_NONE)){
        munmap(addr, PAGESIZE * MAX_PAGE_CLASS); /* sets errno if it fails */
        munmap(free_list, NUMBER_OF_PAGES_FOR_FREE_LIST*PAGESIZE); /* sets errno if it fails */
        pthread_mutex_unlock(&class_array[cls_index].mutex);
        return -1;
    }

    /* free list is a FIFO. It grows backward. We point to the next element to be read */
    class_array[cls_index].free_list = free_list + (NUMBER_OF_PAGES_FOR_FREE_LIST-1)*PAGESIZE;
    class_array[cls_index].free_list_size = 0;
    class_array[cls_index].used = free_list + PAGESIZE;
    class_array[cls_index].used->ptr = NULL;
    pthread_mutex_unlock(&class_array[cls_index].mutex);
    return 0;
}

void insert_in_last_used_list(int cls_index, void* ptr){
    pthread_mutex_lock(&class_array[cls_index].mutex);
    used_t *it = class_array[cls_index].used;
    while(it->next != NULL && it->ptr != NULL)
        it = it->next;

    if(it->next == NULL)
        it->next = it + sizeof(used_t);
    
    if(it->ptr != NULL)
        it = it->next;
    it->ptr = ptr;
    pthread_mutex_unlock(&class_array[cls_index].mutex);
}

/*
 * return 0 if the ptr has been found and -1 otherwise
 */
int delete_in_last_used_list(int cls_index, void* ptr){
    pthread_mutex_lock(&class_array[cls_index].mutex);
    used_t *it = class_array[cls_index].used;

    while(it->next != NULL && (uint64_t) it->ptr != (uint64_t) ptr)
        it = it->next;

    if((uint64_t) it->ptr == (uint64_t) ptr){
        it->ptr = NULL;
        pthread_mutex_unlock(&class_array[cls_index].mutex);
        return 0;
    }
    pthread_mutex_unlock(&class_array[cls_index].mutex);
    return -1;
}

int block_alloced(void *ptr){
    int cls_index = ptrtoclsindex(ptr);
    pthread_mutex_lock(&class_array[cls_index].mutex);
    used_t *it = class_array[cls_index].used;

    while(it->next != NULL && (uint64_t) it->ptr != (uint64_t) ptr)
        it = it->next;

    if((uint64_t) it->ptr == (uint64_t) ptr){
        pthread_mutex_unlock(&class_array[cls_index].mutex);
        return 1;
    }
    pthread_mutex_unlock(&class_array[cls_index].mutex);
    return 0;
}

int clsindex(size_t size){
    if(size == 0) /* we cannot take the log of 0 */
        return -1;
    
    int cls_index = ceil(log2(size + 1)); /* +1 because we need a byte for the cannary */
    if(cls_index <= MAX_CLASS_INDEX)
        return cls_index;
    return MAX_CLASS_INDEX; /* the last class handles bigger allocations */
}

int ptrtoclsindex(void *ptr){
    for(int i = 0; i < MAX_CLASS_INDEX; ++i){
        if(class_array[i].range.first != NULL
           && ((uint64_t) class_array[i].range.first < (uint64_t) ptr
           || (uint64_t) class_array[i].range.second >= (uint64_t) ptr)){ /* if ptr falls into the range */
            return i;
        }
    }
    return MAX_CLASS_INDEX; /* if it is not in another class, it must be a big alloc */
}

void insert_in_free_list(int cls_index, void *ptr, size_t size){/* WARNING it needs to be executed under lock */
    mem_block_t *block = class_array[cls_index].free_list - sizeof(mem_block_t); /* FIFO (grows backward) */
    block->size = size;
    block->ptr = ptr;
    block->next = class_array[cls_index].free_list;
    
    class_array[cls_index].free_list = block; /* add the block to the list */
    ++class_array[cls_index].free_list_size; /* update the size of the list */
}

int addfreespace(int cls_index){
    if(cls_index == MAX_CLASS_INDEX)
        return -1;
    
    size_t PAGESIZE = sysconf(_SC_PAGE_SIZE);

    /* we add enough pages to allocate at least MIN_NUMBER_OF_BLOCK_TO_ALLOC new blocks */
    int number_of_page_to_add = (int) (MIN_NUMBER_OF_BLOCK_TO_ALLOC * pow(2, cls_index) / PAGESIZE) + 1;

    pthread_mutex_lock(&class_array[cls_index].mutex);
    /* we try to mprotect them accordingly */
    if(mprotect(class_array[cls_index].first_page_addr + (class_array[cls_index].last_used_page + 1)*PAGESIZE, PAGESIZE*number_of_page_to_add, PROT_READ | PROT_WRITE) != 0)
        return -1;
    
    /* add the new blocks to the free list */
    size_t size = 2 << (cls_index - 1); /* fast 2^{i} */
    
    void *ptr = class_array[cls_index].first_page_addr + (class_array[cls_index].last_used_page + 1)*PAGESIZE;
    while((size_t) ptr + size < (size_t) (class_array[cls_index].first_page_addr
          + (class_array[cls_index].last_used_page + 1 + number_of_page_to_add)*PAGESIZE)){
        insert_in_free_list(cls_index, ptr, size);
        ptr += size;
    }
    class_array[cls_index].last_used_page += number_of_page_to_add + 1; /* +1 because the page next to it is dedicated to be a guard page */
    
    pthread_mutex_unlock(&class_array[cls_index].mutex);
    return 0;
}


void *cls_malloc(size_t size){
    if(size <= 0) /* it is a protection */
        return NULL;
    
    int cls_index = clsindex(size);
    
    /* checks if classes have been init and init the array if it need to */
    if(class_array == NULL){
        if(initclsarr() != 0)
            return NULL;
    }
    
    /* checks if the class has already been init and init it if it needs to */
    if(class_array[cls_index].free_list == NULL){
        if(initclass(cls_index) != 0)
            return NULL;
    }
    
    
    if(cls_index == MAX_CLASS_INDEX){
        ;
#warning TODO BIG ALLOC
    }

    
    /* check if there is a free block and add some block if there isnt any left */
    if(class_array[cls_index].free_list_size == 0){
        if(addfreespace(cls_index) != 0)
            return NULL;
    }
    
    /*
     * we remove a block from the free list (under lock)
     * we add the new used block (under lock)
     * we set the cannaries (without the lock)
     * we return a pointer to the first byte of the block
     */
    pthread_mutex_lock(&class_array[cls_index].mutex);
    mem_block_t *block = class_array[cls_index].free_list;
    class_array[cls_index].free_list = block->next; /* update the list */
    --class_array[cls_index].free_list_size; /* update the list size */
    pthread_mutex_unlock(&class_array[cls_index].mutex);
    
    /* indicate the address as used to protect out selves from double free */
    insert_in_last_used_list(cls_index, block->ptr);

#warning TODO HASH
    block->ptr[block->size - 1] = ptr_to_cannary(ptr); /* the size has been choosen to beat least size + 1 */
    return block->ptr;
}

/*
 * malloc allocate a size which is a power of two. If the size allocated is the right one, do nothing
 * otherwise, malloc the right size and memcpy the current content of memory
 *
 * The realloc() function tries to change the size of the allocation pointed to by ptr to size, and returns ptr.
 * If there is not enough room to enlarge the memory allocation pointed to by ptr, realloc() creates
 * a new allocation, copies as much of the old data pointed to by ptr as will fit to the new allocation,
 * frees the old allocation, and returns a pointer to the allocated memory.  If ptr is NULL, realloc()
 * is identical to a call to malloc() for size bytes. If size is zero, the original object is freed.
 * The realloc() function does not guarantee that the additional memory is zero-filled.
 */
void* cls_realloc(void* ptr, size_t size){
    if(ptr == NULL)
        return cls_malloc(size);
    
    /* check that the block has indeed been alloced */
    if(block_alloced(ptr) == 0){
        fprintf(stderr, "malloc: *** error for object %p: pointer being realloc'd was not allocated\n", ptr);
        kill(getpid(), SIGBUS);
    }
    
    if(size == 0){
        cls_free(ptr);
        return NULL;
    }
     
    /* looks for the index of the class it belongs to */
    int cls_index_old = ptrtoclsindex(ptr);
    if(cls_index_old == MAX_CLASS_INDEX){
        ;
#warning TODO
    }
    
    /* calculate the index of the class the reallocate value will belongs to */
    int cls_index_new = clsindex(size);
    if(cls_index_new == MAX_CLASS_INDEX){
        ;
#warning TODO
    }
    
    size_t size_old = 2 << (cls_index_old - 1); /* fast 2^{i} */
    size_t size_new = 2 << (cls_index_new - 1); /* fast 2^{i} */
    
    /* check if an overflow occured */
    if(ptr_to_cannary(ptr) != ((byte *) ptr)[size_old - 1]){
        fprintf(stderr, "realloc: *** error for object %p: an overflow occured\n", ptr);
        kill(getpid(), SIGSEGV);
    }

    /* if the two indexes are the same and the allocations are not big alloc, there is nothing to do */
    if(cls_index_new == cls_index_old)
        return ptr;
    
    /*
     * we allocate a new space with malloc
     * we move has much memory as possible there
     * we free the old value
     */
    void *new_ptr = cls_malloc(size);
    
    /* Copy as much mem as possible */
    if(size_old < size_new)
        memcpy(new_ptr, ptr, size_old);
    else
        memcpy(new_ptr, ptr, size_new);
    
    cls_free(ptr);
    return new_ptr;
}


void cls_free(void* ptr){
    int cls_index = ptrtoclsindex(ptr);
    
    if(cls_index == MAX_CLASS_INDEX){
        ;
#warning TODO big alloc
    }
    
    /* check if the block has indeed been allocated */
    if(delete_in_last_used_list(cls_index, ptr) == -1){
        fprintf(stderr, "malloc: *** error for object %p: pointer being freed was not allocated\n", ptr);
        kill(getpid(), SIGABRT);
    }
    
    size_t size = 2 << (cls_index - 1);
    
    if(ptr_to_cannary(ptr) != ((byte *) ptr)[size - 1]){
        fprintf(stderr, "buffer overflow occured at %p, exiting now\n", ptr);
        exit(-1);
    }
    
    pthread_mutex_lock(&class_array[cls_index].mutex);
    insert_in_free_list(cls_index, ptr, size);
    pthread_mutex_unlock(&class_array[cls_index].mutex);
}


byte ptr_to_cannary(void* ptr){
    size_t Value = (size_t) ptr;

    Value = ~Value + (Value << 15);
    Value ^= Value >> 12;
    Value += Value << 2;
    Value ^= Value >> 4;
    Value *= seed;
    Value ^= Value >> 16;

    return (byte) Value;
}
