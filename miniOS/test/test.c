//
//  test.c
//  miniOS
//
//  Created on 18/04/2022.
//

#include <stdio.h>

#include "test.h"
#include "../miniOS.h"
#include "../miniOS_private.h"

void f_t1(void){ //thread 1
    while(1){
        printf("t1\n");
        sleep(1);
    }
}

void f_t2(void){ //thread 2
    while(1){
        printf("t2\n");
        sleep(1);
    }
}


#include "../miniOS_private.h"
extern vCPU *vCPUs;

int main(){
    printf("%d\n", create_vCPU(5));
    config_scheduler(2, RR);
    printf("%d\n", create_uThread(&f_t2, 0, NULL));
    printf("%d\n", create_uThread(&f_t1, 0, NULL));
    printf("%d\n", create_uThread(&f_t1, 0, NULL));
    run();
    return 0;
}
