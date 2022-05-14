////
////  test_hm_legacy.c
////  miniOS
////
////  Created on 29/04/2022.
////
//
////tells gcc that we do not want to know about deprecation.
////without it, some systems complains because contexts are deprecated
//#pragma GCC diagnostic push
//#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
//
//#define M_MMAP_THRESHOLD 128*1024
//
//#include <stdio.h>
//#include <unistd.h>
//#include <errno.h>
//
//
////#include "test_hm_legacy.h"
//#include "memory_legacy.h"
//#include "miniOS.h"
//
//int main(){
//    printf("\033[0;35mThe first batch of tests uses brk allocation and is not secured.\033[0m\n");
//
//    /*
//     * Test 1: allocation using brk
//     */
////    printf("\033[0;32mTesting brk allocation\033[0m\n");
////    printf("Allocating 12 bytes to p1...\n");
//    char *p1 = (char*) hm_malloc(12);
//    if(p1 != NULL){
//        for(int i = 0; i < 12; i++)
//            *(p1 + i) = 'a';
//        printf("\033[0;32mTest 1 works!\033[0m\n");
//    }
//    else
//        printf("\033[0;31mTest 1 fails.\033[0m\n");
//
//
//    /*
//     * Test 2: reallocation using brk when the pointer is the last one
//     */
////    printf("\033[0;32mTesting brk reallocation at end of heap.\033[0m\n");
//    int buff_realloc_brk_end = 1;
//    void * init_ptr = p1;
////    printf("Reallocating 8 bytes to p1...\n");
//    p1 = (char*) hm_realloc(p1, 8);
//    if(p1 != NULL && sbrk(0) == p1 + 8 && p1 == init_ptr){
//        for(int i = 0; i < 8; i++)
//            *(p1 + i) = 'b';
//    }
//    else
//        buff_realloc_brk_end = 0;
//
////    printf("Reallocating 20 bytes to p1...\n");
//    p1 = (char*) hm_realloc(p1, 20);
//    if(p1 != NULL && sbrk(0) == p1 + 20 && p1 == init_ptr){
//        for(int i = 0; i < 20; i++)
//            *(p1 + i) = 'c';
//    }
//    else
//        buff_realloc_brk_end = 0;
////    printf("%p\n%p\n", sbrk(0), p1 + 20);
//
//    if(buff_realloc_brk_end == 0)
//        printf("\033[0;31mTest 2 fails.\033[0m\n");
//    else
//        printf("\033[0;32mTest 2 works!\033[0m\n");
//
//
//    /*
//     * Test 3: a second allocation
//     */
////    printf("\033[0;32mTesting brk allocation when something has already been allocated using brk\033[0m\n");
////    printf("Allocating 30 bytes to p2...\n");
//    char *p2 = (char*) hm_malloc(30);
//    if(p2 != NULL && sbrk(0) == p1 + 20 + sizeof(mem_block) + 30){
//        for(int i = 0; i < 20; i++)
//            *(p1 + i) = 'd';
//        printf("\033[0;32mTest 3 works!\033[0m\n");
//    }
//    else
//        printf("\033[0;31mTest 3 fails.\033[0m\n");
//
//    /*
//     * Test 4: freeing a pointer which is not the last one and has been allocated with brk
//     */
////    printf("\033[0;32mTesting freeing a pointer which is not the last one and has been allocated with brk\033[0m\n");
////    printf("Freeing p1...\n");
//    hm_free(p1);
////    printf("Allocating 8 bytes to p3...\n");
//    char *p3 = (char*) hm_malloc(8);
//    if(   p3 != NULL
//       && p3 == init_ptr)
//        printf("\033[0;32mTest 4 works!\033[0m\n");
//    else
//        printf("\033[0;31mTest 4 fails.\033[0m\n");
//
//    /*
//     * Test 5: freeing the last chuck allocated using brk
//     */
////    printf("\033[0;32mTesting freeing the last chuck allocated using brk\033[0m\n");
//    hm_free(p2);
//    if(sbrk(0) == p3 + 20)
//        printf("\033[0;32mTest 5 works!\033[0m\n");
//    else
//        printf("\033[0;31mTest 5 fails.\033[0m\n");
//
//    /*
//     * Test 6: reallocation using brk when the pointer is in the middle
//     */
////    printf("\033[0;32mTesting brk reallocation using brk when the pointer is in the middle.\033[0m\n");
//    int buff_test_6 = 1;
//    void * init_ptr3 = p3;
////    printf("Allocating 10 bytes to p4...\n");
//    void * p4 = hm_malloc(10);
////    printf("Reallocating 5 bytes to p3...\n");
//    p3 = (char*) hm_realloc(p3, 5);
//    if(p3 != NULL && p3 == init_ptr3){
//        for(int i = 0; i < 5; i++)
//            *(p3 + i) = 'b';
//    }
//    else
//        buff_test_6 = 0;
//
////    printf("Reallocating 50 bytes to p3...\n");
//    p3 = (char*) hm_realloc(p3, 50);
//    if(p3 != NULL && sbrk(0) == p3 + 50 && p3 == p4 + sizeof(mem_block) + 10){
//        for(int i = 0; i < 50; i++)
//            *(p3 + i) = 'c';
//    }
//    else
//        buff_test_6 = 0;
//
//
//    if(buff_test_6 == 0)
//        printf("\033[0;31mTest 6 fails.\033[0m\n");
//    else
//        printf("\033[0;32mTest 6 works!\033[0m\n");
//
//
//
//
//    /*
//     * MMAP ALLOCATION
//     */
//    printf("\033[0;35m\nThe second batch of tests uses mmap allocation and is not secured.\033[0m\n");
//
//    /*
//     * Test 1: allocation using mmap
//     */
////    printf("\033[0;32mTesting mmap allocation\033[0m\n");
////    printf("Allocating M_MMAP_THRESHOLD + 100 bytes to p5...\n");
//    char *p5 = (char*) hm_malloc(M_MMAP_THRESHOLD + 100);
//    if(p5 != NULL){
//        for(int i = 0; i < M_MMAP_THRESHOLD + 100; i++)
//            *(p5 + i) = 'a';
//        printf("\033[0;32mTest 7 works!\033[0m\n");
//    }
//    else
//        printf("\033[0;31mTest 7 fails.\033[0m\n");
//
//
//    /*
//     * Test 2: reallocation using mmap
//     */
////    printf("\033[0;32mTesting mmap reallocation.\033[0m\n");
//    int buff_realloc_brk_end_mmap = 1;
//    
////    printf("Reallocating M_MMAP_THRESHOLD + 200 bytes to p1...\n");
//    p5 = (char*) hm_realloc(p5, 2*M_MMAP_THRESHOLD + 200);
//    if(p5 != NULL){ //can move
//        for(int i = 0; i < 2*M_MMAP_THRESHOLD + 200; i++)
//            *(p5 + i) = 'b';
//    }
//    else
//        buff_realloc_brk_end_mmap = 0;
//
////    printf("Reallocating M_MMAP_THRESHOLD + 80 bytes to p1...\n");
//    void * init_ptr_5 = p5;
//    p5 = (char*) hm_realloc(p5, M_MMAP_THRESHOLD + 80);
//    if(errno == 0 && p5 != NULL && p5 == init_ptr_5){//should not move
//        for(int i = 0; i < M_MMAP_THRESHOLD + 80; i++)
//            *(p5 + i) = 'c';
//    }
//    else
//        buff_realloc_brk_end_mmap = 0;
//    
//
//    if(buff_realloc_brk_end_mmap == 0)
//        printf("\033[0;31mTest 8 fails.\033[0m\n");
//    else
//        printf("\033[0;32mTest 8 works!\033[0m\n");
//
//
//    /*
//     * Test 4: freeing a pointer allocated with mmap
//     */
////    printf("\033[0;32mTesting freeing a pointer allocated with mmap\033[0m\n");
////    printf("Freeing p5...\n");
//    hm_free(p5);
//    if(errno == 0)
//        printf("\033[0;32mTest 9 works!\033[0m\n");
//    else
//        printf("\033[0;31mTest 9 fails.\033[0m\n");
//
//    return 0;
//}
//
//
