#include "time.h"

int __seconds = 0;
int __microseconds = 0;

int __time_get(int * seconds, int * microseconds){
    __microseconds ++;
    if(__microseconds == 1000000){
        __seconds++;
        __microseconds = 0;
    }

    if(seconds != (void *)0) 
        *seconds = __seconds;
    if(microseconds != (void *)0) 
        *microseconds = __microseconds;
    
    return 0;
}
