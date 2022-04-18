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
        sleep(3);
    }
}

void f_t2(void){ //thread 2
    while(1){
        printf("t2\n");
        sleep(3);
    }
}

int main(){
    printf("%d\n", create_vCPU(5));
    sleep(3);
    printf("%d\n", destruct_vCPU(3));
    config_scheduler(1, RR);
    printf("%d\n", create_uThread(&f_t1, 0, NULL));
    
    sleep(1000);
    return 0;
}
