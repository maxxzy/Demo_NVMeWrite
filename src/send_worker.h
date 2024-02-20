#ifndef SENDWORKER_H
#define SENDWORKER_H
#include <mutex>
#include <sys/ipc.h>
#include <sys/msg.h>

struct msg_buf{
    long msg_type;
    char data[4096];
};

#define WRITE_ONCE_BYTE_SIZE 4096

static std::mutex mtx[64];
static const uint64_t kConcurrency_generate = 1;
static const uint64_t kConcurrency = kConcurrency_generate * 4;
static const uint64_t kWriteCountPerThread = 1 * 1;//1000 * 1000;
static const uint64_t kWriteBytesPerThread  = WRITE_ONCE_BYTE_SIZE * kWriteCountPerThread;
static const uint64_t kTotalWriteBytes = kWriteBytesPerThread * kConcurrency;

void send_msg_worker(int index);
void receive_msg(int index);
void del_queue(int index);
void get_data(int index, char *str);

#endif