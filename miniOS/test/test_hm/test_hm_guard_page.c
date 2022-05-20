////
////  test_hm.c
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
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <signal.h>
//
//#include "test_hm.h"
//#include "miniOS.h"
//
//
//
//
//
//void guardpage_works(int i, struct __siginfo * info , void * ptr){
//    printf("\033[0;32mGuard page protection works!\033[0m\n");
//    exit(0);
//}
//
//int main(){
//    printf("\033[0;35mTesting guard page protection.\033[0m\n");
//    static struct sigaction _sigact2;
//    memset(&_sigact2, 0, sizeof(_sigact2));
//    _sigact2.sa_sigaction = guardpage_works;
//    _sigact2.sa_flags = SA_SIGINFO;
//
//    sigaction(SIGBUS, &_sigact2, NULL);
//
//    char *buff = cls_malloc(16000);
//    buff[16385] = 'c';
//
//    printf("\033[0;31mGuard page protection fais.!\033[0m\n");
//        
//    return 0;
//}
//
//
