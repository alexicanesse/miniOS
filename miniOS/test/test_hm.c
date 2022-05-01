//
//  test_hm.c
//  miniOS
//
//  Created on 29/04/2022.
//

//tells gcc that we do not want to know about deprecation.
//without it, some systems complains because contexts are deprecated
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"


#include <stdio.h>
#include <unistd.h>
#include <errno.h>


#include "test_hm.h"
#include "../miniOS.h"



int main(){
    printf("The first batch of tests uses brk allocation.\n");
    
    /*
     * Test 1: allocation using brk
     */
    printf("\033[0;32mTesting brk allocation\033[0m\n");
    printf("Allocating 12 bytes to p1...\n");
    char *p1 = (char*) hm_malloc(12);
    if(p1 != NULL){
        for(int i = 0; i < 12; i++)
            *(p1 + i) = 'a';
        printf("\033[0;35mAllocation using brk works!\033[0m\np1 : %d, %p\n\n", *p1, p1);
    }
    else
        printf("\033[0;31mAllocation using brk fails. :c\033[0m\n\n");

    
    /*
     * Test 2: reallocation using brk when the pointer is the last one
     */
    printf("\033[0;32mTesting brk reallocation at end of heap.\033[0m\n");
    int buff_realloc_brk_end = 1;
    void * init_ptr = p1;
    printf("Reallocating 8 bytes to p1...\n");
    p1 = (char*) hm_realloc(p1, 8);
    if(p1 != NULL && sbrk(0) == p1 + 8 && p1 == init_ptr){
        for(int i = 0; i < 8; i++)
            *(p1 + i) = 'b';
    }
    else
        buff_realloc_brk_end = 0;
    
    printf("Reallocating 20 bytes to p1...\n");
    p1 = (char*) hm_realloc(p1, 20);
    if(p1 != NULL && sbrk(0) == p1 + 20 && p1 == init_ptr){
        for(int i = 0; i < 20; i++)
            *(p1 + i) = 'c';
    }
    else
        buff_realloc_brk_end = 0;
    
    
    if(buff_realloc_brk_end == 0)
        printf("\033[0;31mReallocation using brk at end of heap fails. :c\033[0m\n\n");
    else
        printf("\033[0;35mReallocation using brk works!\033[0m\np1 : %d, %p\n\n", *p1, p1);
    
    
    /*
     * Test 3: a second allocation
     */
    printf("\033[0;32mTesting brk allocation when something has already been allocated using brk\033[0m\n");
    printf("Allocating 30 bytes to p2...\n");
    char *p2 = (char*) hm_malloc(30);
    if(p2 != NULL && sbrk(0) == p1 + 20 + sizeof(mem_block) + 30){
        for(int i = 0; i < 30; i++)
            *(p1 + i) = 'd';
        printf("\033[0;35mA second allocation using brk works!\033[0m\np1 : %d, %p\n\n", *p1, p1);
    }
    else
        printf("\033[0;31mA second allocation using brk fails. :c\033[0m\n\n");
    
    /*
     * Test 4: freeing a pointer which is not the last one and has been allocated with brk
     */
    printf("\033[0;32mTesting freeing a pointer which is not the last one and has been allocated with brk\033[0m\n");
    printf("Freeing p1...\n");
    hm_free(p1);
    printf("Allocating 8 bytes to p3...\n");
    char *p3 = (char*) hm_malloc(8);
    printf("Allocating 12 bytes to p4...\n");
    char *p4 = (char*) hm_malloc(12);
    if(   p3 != NULL
       && p4 != NULL
       && p3 == init_ptr
       && p3 + 8 == p4
       && p4 + 12 == p2)
        printf("\033[0;35mFreeing a pointer which is not the last one and has been allocated with brk works!\033[0m\np1 : %d, %p\n\n", *p1, p1);
    else
        printf("\033[0;31mFreeing a pointer which is not the last one and has been allocated with brk fails. :c\033[0m\n\n");
    
    /*
     * Test 5: freeing the last chuck allocated using brk
     */
    printf("\033[0;32mTesting freeing the last chuck allocated using brk\033[0m\n");
    hm_free(p2);
    if(sbrk(0) == p4 + 12)
        printf("\033[0;35mFreeing the last chuck allocated using brk works!\033[0m\np1 : %d, %p\n\n", *p1, p1);
    else
        printf("\033[0;31mFreeing the last chuck allocated using brk fails. :c\033[0m\n\n");
    
//
//    printf("Allocate 20 bytes to p2...\n");
//    int *p2 = (int *) hm_malloc(20);
//    printf("p2 : %d, %p\n", *p2, p2);
//
//    printf("Set p2 to 69...\n");
//    *p2 = 69;
//    printf("p2 : %d, %p\n", *p2, p2);
//
//    printf("Free p1...\n");
//    hm_free(p1);
//
//
//    printf("Allocate 30 bytes to p3...\n");
//    int *p3 = (int *) hm_malloc(30);
//    printf("p3 : %d, %p\n", *p3, p3);
//    printf("Free p3...\n");
//    hm_free(p3);
//
//    printf("Allocate 12 bytes to p4...\n");
//    int *p4 = (int *) hm_malloc(12);
//    printf("p4 : %d, %p\n", *p4, p4);
//
//
//    printf("\n******************************\n\n");
//    printf("The second batch of tests uses mmap allocation.\n");
//
//
//    printf("Allocate 1400000000 bytes to p5...\n");
//    int *p5 = (int *) hm_malloc(1400000000);
//    printf("Reallocate 1400000000 bytes to p5...\n");
//    p5 = (int *) hm_realloc(p5, 1400000000);
//    printf("Reallocate 1400000000 bytes to p5...\n");
//    p5 = (int *) hm_realloc(p5, 1400000000);
//    printf("p5 : %d, %p\n", *p5, p5);
//
//    printf("Set p5 to 42...\n");
//    *p5 = 42;
//    printf("%d, %p\n", *p5, p5);
//
//
//    printf("Set p5 to 42...\n");
//    *p5 = 42;
//    printf("%d, %p\n", *p5, p5);
//
//    printf("Allocate 160000 bytes to p6...\n");
//    int *p6 = (int *) hm_malloc(160000);
//    printf("p6 : %d, %p\n", *p6, p6);
//
//    printf("Set p6 to 69...\n");
//    *p6 = 69;
//    printf("p6 : %d, %p\n", *p6, p6);
//
//    printf("Free p5...\n");
//    hm_free(p5);
//
//
//    printf("Allocate 200000 bytes to p3...\n");
//    int *p7 = (int *) hm_malloc(200000);
//    printf("p7 : %d, %p\n", *p7, p7);
//
//
//    printf("Allocate 139000 bytes to p8...\n");
//    int *p8 = (int *) hm_malloc(1390000);
//    printf("p8 : %d, %p\n", *p8, p8);
//
//
//    printf("\n******************************\n\n");
//    printf("The last batch of tests uses mmap and break allocation.\n");
//
//    printf("Free p2...\n");
//    hm_free(p2);
//
//    printf("Allocate 5 bytes to p9...\n");
//    int *p9 = (int *) hm_malloc(5);
//    printf("p9 : %d, %p\n", *p9, p9);
//
//
//    printf("Allocate 6 bytes to p10...\n");
//    int *p10 = (int *) hm_malloc(6);
//    printf("p10 : %d, %p\n", *p10, p10);
//
//    printf("\n******************************\n\n");
//    printf("\nResult:\n\n");
//    printf("p1 : freed\n");
//    printf("p2 : freed\n");
//    printf("p3 : freed\n");
//    printf("p4 : %d, %p\n", *p4, p4);
//    printf("p5 : freed\n");
//    printf("p6 : %d, %p\n", *p6, p6);
//    printf("p7 : %d, %p\n", *p7, p7);
//    printf("p8 : %d, %p\n", *p8, p8);
//    printf("p9 : %d, %p\n", *p9, p9);
//    printf("p10 : %d, %p\n", *p10, p10);
//    printf("brk address: %p\n", sbrk(0));
//    sleep(100);
    return 0;
}


