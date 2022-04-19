//
//  test.c
//  miniOS
//
//  Created on 18/04/2022.
//

#include <stdio.h>

#include "test.h"
#include "../miniOS.h"

void f_t1(void){ //thread 1
    while(1){
        printf("t1\n");
//        sleep(3);
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
    printf("%d\n", create_vCPU(3));
    config_scheduler(1, RR);
    printf("%d\n", create_uThread(&f_t2, 0, NULL));
    run();
    while(1){
        pthread_kill(*(vCPUs->pthread), SIGALRM);
        sleep(5);
    }
    sleep(1000);
    return 0;
}
