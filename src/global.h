#ifndef GLOBAL_H
#define GLOBAL_H
#include <mutex>
#include <QMutexLocker>
#include <iostream>
#include <atomic>

#define likely(x)      __builtin_expect(!!(x), 1)
#define unlikely(x)    __builtin_expect(!!(x), 0)
#define WRITE_ONCE_BYTE_SIZE 4096

static std::string diskPath[4] = {
    "./datafile/data",
    "./datafile/data",
    "./datafile/data",
    "./datafile/data"
};

static std::mutex mtx[64];
static std::mutex statMtx;

extern std::atomic<uint64_t> written_count;
extern uint64_t WriteBytesStat;
extern uint64_t kConcurrency_generate;
extern uint64_t kConcurrency;
extern uint64_t kWriteCountPerThread;
extern uint64_t kWriteBytesPerThread;
extern uint64_t kTotalWriteBytes;
extern uint64_t kStagingFileSize;
extern int IOMethod;
extern int NVMeNumber;
extern void (*writeFunc[3])(int, int);

uint64_t NowMicros();

#endif