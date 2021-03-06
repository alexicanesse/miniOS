//
//  memory_legacy.c
//  miniOS
//
//  Created on 14/05/2022.
//

#include "memory_legacy.h"

/*
 * recommanded default value
 * must be a power of two to be fully respected
 */
#define M_MMAP_THRESHOLD 128*1024


//tells gcc that we do not want to know about deprecation.
//without it, some systems complains because contexts are deprecated
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"


#include <unistd.h>
#include <pthread.h>
#include <sys/types.h> //requiered for mmap
#include <sys/mman.h>
#include <string.h>
#include <math.h>


pthread_mutex_t mutex_mem_list = PTHREAD_MUTEX_INITIALIZER; /* lock used to make some ressources thread-safe */
pthread_mutex_t mutex_mem_list_secured = PTHREAD_MUTEX_INITIALIZER; /* lock used to make some ressources thread-safe */

mem_block *mem_list = NULL;
mem_block *last_brk = NULL;



/*
 * This function allocated a page secured page page guards to store
 * the metadata of allocations
 */
mem_block *init_mem_block(void){
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
    
    if(address == MAP_FAILED)
        return NULL;
    
    /* set guard pages */
    if(mprotect(address, PAGESIZE, PROT_NONE) || mprotect(address + 2*PAGESIZE, PAGESIZE, PROT_NONE)){
        munmap(address, 3*PAGESIZE); /* sets errno if it fails */
        return NULL;
    }
    address += PAGESIZE;
    
    mem_block *block = address;
    block->size = 0;
    block->ptr = NULL;
    block->is_secure = 0;
    block->is_used = 0;
    block->prev = NULL;
    block->is_brk = 0;
    block->next = block;
    return address;
}


//be careful, this is not thread safe
void insert_block(mem_block *block){
    //we insert the block in a way that keeps the list order by increasing address
    int inserted = 0;
    mem_block *mem_block_it = mem_list;
    mem_block *last = NULL;
    while(mem_block_it != NULL && inserted == 0){
        if(((long int) mem_block_it->ptr) >= ((long int) block->ptr)){//the first one to reverse the order
            ++inserted;
            block->prev = mem_block_it->prev;
            block->next = mem_block_it;
            block->next->prev = block;
            if(block->prev != NULL)
                block->prev->next = block;
            else
                mem_list = block; //it needs to be the first element
        }
        if(mem_block_it->next == NULL) //last element
            last = mem_block_it;

        mem_block_it = mem_block_it->next;
    }
    if(inserted == 0){//it hasn't been inserted, we insert it at the end
        if(last == NULL)//empty list
            mem_list = block;
        else{
            last->next = block;
            block->prev = last;
        }
    }
    
    if(block->is_brk == 1 && (last_brk == NULL || last_brk->ptr < block->ptr))
        last_brk = block;
}


void *hm_malloc(size_t size){
    /*
     * We look for a block big enough
     */
    pthread_mutex_lock(&mutex_mem_list);
    void* address = NULL;
    mem_block *mem_block_it = mem_list;
    while(mem_block_it != NULL && address == NULL){//as long as we can iterate and haven't found any block
        if(mem_block_it->is_used == 0 && mem_block_it->size >= size){ //the block is big enough.
            address = mem_block_it->ptr;

            /*
             * if the size is big enough to slice it, we slice it
             * there is not any sanitary check to ensure the block has been
             * allocated using sbrk. Though, it cannot have been allocated using
             * mmap because it would have been numap when it has been freed
             */
            if(mem_block_it->size > size + sizeof(mem_block)){
                //we create the new block and insert it
                mem_block *block = (mem_block *) address + size;
                block->ptr = address + size + sizeof(mem_block);
                block->size = mem_block_it->size - size - sizeof(mem_block);
                block->is_used = 0;
                block->is_brk = mem_block_it->is_brk;
                block->is_secure = 0;
                block->next = mem_block_it->next;
                block->prev = mem_block_it;


                mem_block_it->size = size;
                mem_block_it->next = block;

                if(last_brk == mem_block_it)
                    last_brk = block;
            }
            mem_block_it->is_used = 1;
        }
        mem_block_it = mem_block_it->next;
    }

    /*
     * There is not any block big enough
     */
    if(address == NULL){
        int is_brk = 0;
        if(size < M_MMAP_THRESHOLD){
            /*
             * sbrk is deprecated and should not be used
             * we should not use it
             * but we use it anyway because we have to
             */
            is_brk = 1;
            address = sbrk(size + sizeof(mem_block)); //sbrk return the address before the extension. The address we will use
            if(address == (void *) -1) //sbrk failed
                return NULL;
        }
        else{
            address = mmap(0x0, /* the kernel is free to choose where to map it */
                 size + sizeof(mem_block),
                 PROT_READ | PROT_WRITE, /* we can do whatever we want with it */
                 MAP_SHARED | MAP_ANONYMOUS,
                 0,
                 0);
            if(address == MAP_FAILED)
                return NULL;
        }

        //we create the new block and insert it
        mem_block *block = (mem_block *) address;
        address += sizeof(mem_block);
        block->ptr = address;
        block->size = size;
        block->is_used = 1;
        block->is_brk = is_brk;
        block->is_secure = 0;
        block->next = NULL;
        block->prev = NULL;
        insert_block(block);

        if(is_brk)
            last_brk = block;
    }
    pthread_mutex_unlock(&mutex_mem_list);
    return address;
}


void *hm_realloc(void* ptr, long int size){
    //we find the corresponding block if it exists
    pthread_mutex_lock(&mutex_mem_list);
    mem_block *mem_block_it = mem_list;
    while(mem_block_it != NULL){
        if(mem_block_it->ptr == ptr){//we found it!
            if(mem_block_it->size > size){ //we can use this block
                if(mem_block_it->is_brk){//allocated using the break pointer
                    if(last_brk == mem_block_it){ //if it is on top of the heap, we realease the memory
                        sbrk(size - mem_block_it->size);
                        mem_block_it->size = size;
                    }
                    else if(mem_block_it->size > size + sizeof(mem_block)){ //we split the memory left
                        //we create the new block and insert it
                        mem_block *block = (mem_block *) mem_block_it->ptr + size;
                        block->ptr = mem_block_it->ptr + size + sizeof(mem_block);
                        block->size = mem_block_it->size - size - sizeof(mem_block);
                        block->is_used = 0;
                        block->is_brk = 1;
                        block->is_secure = 0;
                        block->next = mem_block_it->next;
                        block->prev = mem_block_it;

                        //we change the size of the previous block
                        mem_block_it->size = size;
                        mem_block_it->next = block;
                    }
                }
                else{//allocated using mmap
                    /*
                     * we release the memory that we do not need anymore
                     * we change the size of the previous block if munmap worked
                     */
                    //find the next page
                    long int page_size = sysconf(_SC_PAGESIZE);
                    void * nxt_page = (void *) ((((long int) (mem_block_it->ptr + size))/page_size + 1)*page_size);
                    
                    //if the next page is mapped
                    if(mem_block_it->size - size > page_size
                       && (munmap(nxt_page, mem_block_it->size - size) == 0)) //if it fails it sets erno
                        mem_block_it->size = (long int) nxt_page - (long int) mem_block_it->ptr;
                }
            }
            else{//we need to enlarge the memory block
                if(mem_block_it->is_brk){//allocated using the break pointer
                    //if the next block is free, we fusion our blocks until out block is big enough
                    while(mem_block_it->size < size
                          && mem_block_it->next != NULL
                          && mem_block_it->next->is_used == 0)
                        fusion_with_next(mem_block_it);

                    //if the block is big enough to be used, we call realloc again and the first part will handle it
                    if(mem_block_it->size >= size){
                        pthread_mutex_unlock(&mutex_mem_list);
                        return hm_realloc(mem_block_it->ptr, size);
                    }
                    
                    if(last_brk == mem_block_it){//if the block is the last block
                        //we just move the break pointer
                        if(mem_block_it == last_brk){
                            if(sbrk(size - mem_block_it->size) != (void *) -1) //if it worked
                                mem_block_it->size = size;
                        }
                    }
                    else{
                        /*
                        * if it cannot be enlarged enough and is not the last block
                        * we must malloc a new block and free up this one after a memcpy
                         */
                        pthread_mutex_unlock(&mutex_mem_list); //malloc and free need the lock to be lifted
                        void *ptr = hm_malloc(size);
                        memcpy(ptr, mem_block_it->ptr, mem_block_it->size); //the memory is moved to the new location
                        hm_free(mem_block_it->ptr); //the precedent location is freed
                        return ptr;
                    }
                }
                else{//allocated using mmap
                    /*
                     * we malloc anough memory
                     * we copy the current memory inside the new one
                     * we free the old memory
                     * we return the new one
                     */
                    pthread_mutex_unlock(&mutex_mem_list); //malloc and free need the lock to be lifted
                    void *ptr = hm_malloc(size);
                    memcpy(ptr, mem_block_it->ptr, mem_block_it->size); //the memory is moved to the new location
                    hm_free(mem_block_it->ptr); //the precedent location is freed
                    return ptr;
                }
            }
            pthread_mutex_unlock(&mutex_mem_list);
            return mem_block_it->ptr;
        }
        mem_block_it = mem_block_it->next;
    }
    pthread_mutex_unlock(&mutex_mem_list);

    //we have not find the block; we just allocate a new block of memory;
    return hm_malloc(size);
}


void hm_free(void *ptr){
    //we find the block
    pthread_mutex_lock(&mutex_mem_list);
    mem_block *mem_block_it = mem_list;
    while(mem_block_it != NULL){
        if(mem_block_it->ptr == ptr){//we found it!
            if(mem_block_it->is_brk){ //alloced with sbrk
                /*
                 * We free it if it is the last chuck of memory
                 * of any adjacent block is free, we fusion them
                 */
                if(mem_block_it == last_brk){
                    while(mem_block_it != NULL && mem_block_it->prev->is_used == 0){
                        mem_block_it = mem_block_it->prev;
                        fusion_with_next(mem_block_it);
                    }
                    
                    sbrk( - sizeof(mem_block) - mem_block_it->size);
                    if(mem_block_it->prev == NULL){//first block
                        mem_list = mem_block_it->next;
                        if(mem_block_it->next != NULL)
                            mem_block_it->next->prev = NULL;
                    }
                    else{
                        mem_block_it->prev->next = mem_block_it->next;
                        if(mem_block_it->next != NULL)
                            mem_block_it->next->prev = mem_block_it->prev;
                    }
                    last_brk = mem_block_it->prev;
                }
                else{
                    mem_block_it->is_used = 0;
                    while(mem_block_it->prev != NULL && mem_block_it->prev->is_used == 0){
                        mem_block_it = mem_block_it->prev;
                        fusion_with_next(mem_block_it);
                    }
                    while(mem_block_it->next != NULL && mem_block_it->next->is_used == 0)
                        fusion_with_next(mem_block_it);
                }
            }
            else{ //alloced with mmap
                /*
                 * Thoses are huge chuck; we free them from memory
                 */
                if(mem_block_it->prev == NULL)//first block
                    mem_list = mem_block_it->next;
                else{
                    mem_block_it->prev->next = mem_block_it->next;
                    if(mem_block_it->next != NULL)
                        mem_block_it->next->prev = mem_block_it->prev;
                }
                munmap(mem_block_it, sizeof(mem_block) + mem_block_it->size); //if it fails it sets erno
            }
            pthread_mutex_unlock(&mutex_mem_list);
            return;
        }
        mem_block_it = mem_block_it->next;
    }
    pthread_mutex_unlock(&mutex_mem_list);
}


void fusion_with_next(mem_block *block){
    /*
     * This function must only be called we a break block
     * as blocks are ordered and break blocks are contigous, this is valid
     */

    if(block == NULL || block->next == NULL)
        return;


    if(block->is_brk == 0 || block->next->is_brk == 0) //check that the block and the following are break blocks
        return;


    //if the next block is the last block
    if(block->next == last_brk)
        last_brk = block;

    

    block->size = block->size + block->next->size + sizeof(mem_block);
    block->next = block->next->next;

}
