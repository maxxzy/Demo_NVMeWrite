#include "global.h"
#include <sys/time.h>
#include "write_worker.h"

uint64_t WriteBytesStat = 0;
uint64_t kConcurrency_generate = 16;
uint64_t kConcurrency = kConcurrency_generate * 4;
uint64_t kWriteCountPerThread = 200 * 200;
uint64_t kWriteBytesPerThread = WRITE_ONCE_BYTE_SIZE * kWriteCountPerThread;
uint64_t kTotalWriteBytes = kWriteBytesPerThread * kConcurrency;
uint64_t kStagingFileSize = WRITE_ONCE_BYTE_SIZE * 4;
int IOMethod = 0;
int NVMeNumber = 0;
void (*writeFunc[3])(int, int) = {buffer_io, direct_io, mmap_io};

uint64_t NowMicros(){
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return static_cast<uint64_t>(tv.tv_sec) * 1000000 + tv.tv_usec;
}