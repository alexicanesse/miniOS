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
//
//
//int main(){
//    char *buff = cls_malloc(14);
//    buff[15] = 'c';
//    printf("\033[0;35mIf an overflow occured, it worked.\033[0m\n");
//    cls_free(buff);
//    
//    
//    printf("\033[0;31mOverflow protection fais.!\033[0m\n");
//        
//    return 0;
//}
//
//
