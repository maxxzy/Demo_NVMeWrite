#include "global.h"
#include <sys/time.h>

uint64_t WriteBytesStat = 0;

uint64_t NowMicros(){
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return static_cast<uint64_t>(tv.tv_sec) * 1000000 + tv.tv_usec;
}