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

#define M_MMAP_THRESHOLD 128*1024

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "test_hm.h"
#include "miniOS.h"



int main(){
    printf("\033[0;35mTesting allocating a lot using malloc.\033[0m\n");
    size_t malloced = 0;

    int i = 4;
    while(i--){
        size_t size = random() & 0x11111111111;
        char* p = cls_malloc(size); /* big malloc */
        if(p == NULL){
            printf("\033[0;31mMalloc failed after the allocation of %zu bytes.\033[0m\n", malloced);
            break;
        }
        for(int j = 0; j < size; j++)
            p[j] = 'p';
        
        malloced += size;
        size = random() & 0x111111111;
        p = cls_malloc(size); /* small malloc */
        if(p == NULL){
            printf("\033[0;31mMalloc failed after the allocation of %zu bytes.\033[0m\n", malloced);
            break;
        }
        for(int j = 0; j < size; j++)
            p[j] = 'p';
        malloced += size;
    }
    
    if(i == -1)
        printf("\033[0;32mSuccessfully alloced %zu bytes!\033[0m\n", malloced);
    
    
    char *p = cls_malloc(10);
    for(int i = 0; i < 10; i++)
        p[i] = 'a';
    
    p = cls_realloc(p, 100);
    for(int i = 0; i < 10; i++)
        if(p[i] != 'a')
//            printf("\033[0;31mRealloc fails.!\033[0m\n");
    for(int i = 0; i < 100; i++)
        p[i] = 'b';
    for(int i = 0; i < 100; i++)
        if(p[i] != 'b')
            printf("\033[0;31mRealloc fails.!\033[0m\n");

    p = cls_realloc(p, 10000);
    for(int i = 0; i < 100; i++)
        if(p[i] != 'b')
            printf("\033[0;31mRealloc fails.!\033[0m\n");
    for(int i = 0; i < 10000; i++)
        p[i] = 'c';
    for(int i = 0; i < 10000; i++)
        if(p[i] != 'c')
            printf("\033[0;31mRealloc fails.!\033[0m\n");
    
    p = cls_realloc(p, 10);
    for(int i = 0; i < 10; i++)
        if(p[i] != 'c')
            printf("\033[0;31mRealloc fails.!\033[0m\n");
    for(int i = 0; i < 10; i++)
        p[i] = 'd';
    
    for(int i = 0; i < 10; i++)
        if(p[i] != 'd')
            printf("\033[0;31mRealloc fails.!\033[0m\n");
    cls_free(p);
    
    printf("\033[0;32mRealloc and free work!\033[0m\n"); /* there would have been an error if it did not work */

    return 0;
}


