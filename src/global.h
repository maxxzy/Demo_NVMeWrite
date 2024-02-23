#ifndef GLOBAL_H
#define GLOBAL_H
#include <mutex>
#include <QMutexLocker>

#define likely(x)      __builtin_expect(!!(x), 1)
#define unlikely(x)    __builtin_expect(!!(x), 0)
#define WRITE_ONCE_BYTE_SIZE 4096

static char test_data[WRITE_ONCE_BYTE_SIZE] __attribute__((aligned(WRITE_ONCE_BYTE_SIZE))) = {'a'};

static std::mutex mtx[64];
static std::mutex statMtx;
extern uint64_t WriteBytesStat;
static const uint64_t kConcurrency_generate = 16;
static const uint64_t kConcurrency = kConcurrency_generate * 4;
static const uint64_t kWriteCountPerThread = 200 * 200;//1000 * 1000;
static const uint64_t kWriteBytesPerThread  = WRITE_ONCE_BYTE_SIZE * kWriteCountPerThread;
static const uint64_t kTotalWriteBytes = kWriteBytesPerThread * kConcurrency;
static const uint64_t kStagingFileSize = WRITE_ONCE_BYTE_SIZE * 4;

uint64_t NowMicros();

#endif