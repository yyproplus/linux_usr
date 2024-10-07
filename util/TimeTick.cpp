#include "TimeTick.h"
#include <sys/time.h>
#include <string.h>

u64 GetTickTime(){
    struct timeval tv;
    memset(&tv,0,sizeof(struct timeval));
    gettimeofday(&tv,NULL);
    return (tv.tv_sec*1000+tv.tv_usec/1000);
}