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
    printf("here\n");
//    int i = 10;
//    while(i--){
//        printf("%d\n", i);
////        sleep(1);
//    }
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
    printf("%d\n", create_vCPU(1));
    config_scheduler(2, RR);
    run();
    printf("%d\n", create_uThread(&f_t2, 0, NULL));
//    printf("%d\n", create_uThread(&f_t1, 0, NULL));
    while(1){
//        printf("%d\n", create_uThread(&f_t1, 0, NULL));
        create_uThread(&f_t1, 0, NULL);
        sleep(1);
    }

    idle();
    return 0;
}
