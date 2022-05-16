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

#include "test_hm.h"
#include "miniOS.h"


#include <stdio.h>
#include <stdlib.h>

int main(){
    void *p = cls_malloc(10);
    void *t = cls_malloc(10);
    printf("%p\n%p\n", p, t);
    
    cls_free(p);
    cls_free(p);
    return 0;
}


