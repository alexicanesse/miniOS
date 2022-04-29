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

#include "test_hm.h"
#include "../miniOS.h"



int main(){
    printf("The first batch of tests uses brk allocation.\n");
    
    printf("Allocate 12 bytes to p1...\n");
    int *p1 = (int *) hm_malloc(12);
    printf("p1 : %d, %p\n", *p1, p1);
    
    printf("Set p1 to 42...\n");
    *p1 = 42;
    printf("%d, %p\n", *p1, p1);
    
    printf("Allocate 20 bytes to p2...\n");
    int *p2 = (int *) hm_malloc(20);
    printf("p2 : %d, %p\n", *p2, p2);
    
    printf("Set p2 to 69...\n");
    *p2 = 69;
    printf("p2 : %d, %p\n", *p2, p2);
    
    printf("Free p1...\n");
    hm_free(p1);
    
    
    printf("Allocate 30 bytes to p3...\n");
    int *p3 = (int *) hm_malloc(30);
    printf("p3 : %d, %p\n", *p3, p3);
    
    
    printf("Allocate 12 bytes to p4...\n");
    int *p4 = (int *) hm_malloc(12);
    printf("p4 : %d, %p\n", *p4, p4);
    
    
    printf("\n******************************\n\n");
    printf("The second batch of tests uses mmap allocation.\n");

    
    printf("Allocate 40000 bytes to p5...\n");
    int *p5 = (int *) hm_malloc(40000);
    printf("p5 : %d, %p\n", *p5, p5);
    
    printf("Set p5 to 42...\n");
    *p5 = 42;
    printf("%d, %p\n", *p5, p5);
    
    printf("Allocate 60000 bytes to p6...\n");
    int *p6 = (int *) hm_malloc(60000);
    printf("p6 : %d, %p\n", *p6, p6);
    
    printf("Set p6 to 69...\n");
    *p6 = 69;
    printf("p6 : %d, %p\n", *p6, p6);
    
    printf("Free p5...\n");
    hm_free(p5);
    
    
    printf("Allocate 100000 bytes to p3...\n");
    int *p7 = (int *) hm_malloc(100000);
    printf("p7 : %d, %p\n", *p7, p7);
    
    
    printf("Allocate 40000 bytes to p8...\n");
    int *p8 = (int *) hm_malloc(40000);
    printf("p8 : %d, %p\n", *p8, p8);

    
    printf("\n******************************\n\n");
    printf("The last batch of tests uses mmap and break allocation.\n");
    
    printf("Free p2...\n");
    hm_free(p2);
    
    printf("Allocate 5 bytes to p9...\n");
    int *p9 = (int *) hm_malloc(5);
    printf("p9 : %d, %p\n", *p9, p9);
    
    
    printf("Allocate 6 bytes to p10...\n");
    int *p10 = (int *) hm_malloc(6);
    printf("p10 : %d, %p\n", *p10, p10);
    
    printf("\n******************************\n\n");
    printf("\nResult:\n\n");
    printf("p1 : freed\n");
    printf("p2 : freed\n");
    printf("p3 : %d, %p\n", *p3, p3);
    printf("p4 : %d, %p\n", *p4, p4);
    printf("p5 : freed\n");
    printf("p6 : %d, %p\n", *p6, p6);
    printf("p7 : %d, %p\n", *p7, p7);
    printf("p8 : %d, %p\n", *p8, p8);
    printf("p9 : %d, %p\n", *p9, p9);
    printf("p10 : %d, %p\n", *p10, p10);
    
    return 0;
}


