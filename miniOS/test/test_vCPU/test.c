//
//  test.c
//  miniOS
//
//  Created on 18/04/2022.
//



#include <stdio.h>
#include <unistd.h>

#include "test.h"
#include "miniOS.h"

void f_t1(void){ //thread 1
    int i = 10;
    while(i--){
        printf("\033[0;34m-t1-%d--------- \033[0m\n", i);
    }
}

void f_t2(void){ //thread 2
    while(1){
        printf("\033[0;36m--------t2----\033[0m\n");
        sleep(1);
    }
}

void f_t3(void){ //thread 2
    while(1){
        printf("\033[0;33m-----------t3-\033[0m\n");
        sleep(2);
    }
}


int main(){
    printf("%d\n", create_vCPU(1));
    config_scheduler(2, RR);
    run();
    printf("%d\n", create_uThread(&f_t2, 0, NULL));
    printf("%d\n", create_uThread(&f_t3, 0, NULL));
    printf("%d\n", create_uThread(&f_t1, 0, NULL));
    while(1){
        char c;
        scanf("%c", &c);
        create_uThread(&f_t1, 0, NULL);
    }
    return 0;
}
