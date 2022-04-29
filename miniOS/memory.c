//
//  memory.c
//  miniOS
//
//  Created on 15/04/2022.
//

//recommanded default value
#define M_MMAP_THRESHOLD 128*1024

#include <sys/mman.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>

#include "memory.h"


void fusion_with_next(mem_block *block);




pthread_mutex_t mutex_mem_list = PTHREAD_MUTEX_INITIALIZER; //lock use to make some ressources thread-safe

mem_block *mem_list = NULL;
mem_block *last_brk = NULL;


//be careful, this is not thread safe
void insert_block(mem_block *block){
    //we insert the block in a way that keeps the list order by increasing address
    int inserted = 0;
    mem_block *mem_block_it = mem_list;
    mem_block *last = NULL;
    while(mem_block_it != NULL && inserted != 0){
        if(mem_block_it->ptr >= block->ptr){//the first one to reverse the order
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
}


void *hm_malloc(long int size){
    /*
     * We look for a block big enough
     */
    pthread_mutex_lock(&mutex_mem_list);
    void* address = NULL;
    
    mem_block *mem_block_it = mem_list;
    while(mem_block_it != NULL && address == NULL){
        if(mem_block_it->is_used == 0 && mem_block_it->size >= size){ //the block is big enough.
            address = mem_block_it->ptr;

            //if the size is big enough to slice it, we slice it
            if(mem_block_it->size > size + sizeof(mem_block)){
                
                //we create the new block and insert it
                mem_block *block = (mem_block *) address + mem_block_it->size;
                block->ptr = address + mem_block_it->size + sizeof(mem_block);
                block->size = mem_block_it->size - size - sizeof(mem_block);
                block->is_used = 0;
                block->is_brk = mem_block_it->is_brk;
                block->next = mem_block_it->next;
                block->prev = mem_block_it;
               
                
                mem_block_it->size = size;
                mem_block_it->next = block;
                
                if(last_brk == mem_block_it)
                    last_brk = block;
            }
        }
        mem_block_it = mem_block_it->next;
    }
    
    if(address == NULL){ //there is not any block big enough
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
            void * address = mmap(NULL /* the kernel is free to choose where to map it */,
                 size + sizeof(mem_block),
                 PROT_READ | PROT_WRITE | PROT_EXEC, /* we can do whatever we want with it */
                 MAP_PRIVATE|MAP_ANONYMOUS,
                 -1,
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
            if(mem_block_it->size > size + sizeof(mem_block)){ //we can juste split the memory block
                if(mem_block_it->is_brk){//allocated using the break pointer
                    if(last_brk == mem_block_it) //if it is on to of the heap, we realease the memory
                        sbrk(size - mem_block_it->size);
                    else{ //we split the memory left
                        //we create the new block and insert it
                        mem_block *block = (mem_block *) mem_block_it->ptr + size;
                        block->ptr = mem_block_it->ptr + size + sizeof(mem_block);
                        block->size = mem_block_it->size - size - sizeof(mem_block);
                        block->is_used = 0;
                        block->is_brk = 1;
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
                    if(munmap(mem_block_it->ptr + size, mem_block_it->size - size) == 0) //if it fails it sets erno
                        mem_block_it->size = size;
                }
            }
            else{//we need to enlarge the memory block
                if(mem_block_it->is_brk){//allocated using the break pointer
                    if(last_brk == mem_block_it){//we just move the break pointer
                        //if the next block is free, we fusion our blocks until out block is big enough
                        while(mem_block_it->size < size
                              && mem_block_it->next != NULL
                              && mem_block_it->next->is_used == 0)
                            fusion_with_next(mem_block_it);
                        
                        //if the block is big enough to be sliced, we call malloc again and the first part will handle it
                        if(mem_block_it->size > size + sizeof(mem_block)){
                            pthread_mutex_unlock(&mutex_mem_list);
                            return hm_malloc(size);
                        }
                        
                        //if the block is the last block
                        if(mem_block_it == last_brk){
                            if(sbrk(size - mem_block_it->size) != (void *) -1) //if it worked
                                mem_block_it->size = size;
                        }
                        
                        /*
                        * if it cannot be enlarged enough and is not the last block
                        * we must malloc a new block and free up this one after a memcpy
                         */
                        pthread_mutex_unlock(&mutex_mem_list); //malloc and free need the lock to be lifted
                        void *ptr = hm_malloc(size);
                        memcpy(ptr, mem_block_it->ptr, mem_block_it->size); //the memory is moved to the new location
                        hm_free(mem_block_it->ptr); //the precedent location is freed
                        pthread_mutex_lock(&mutex_mem_list); //it doesn't matter if another thread entered a restricted area during this time
                        return ptr;
                    }
                }
                else{//allocated using mmap
                    
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
                    sbrk( - sizeof(mem_block) - mem_block_it->size);
                    if(mem_block_it->prev == NULL)//first block
                        mem_list = mem_block_it->next;
                    else{
                        mem_block_it->prev->next = mem_block_it->next;
                        mem_block_it->next->prev = mem_block_it->prev;
                    }
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
