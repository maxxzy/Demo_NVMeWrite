#ifndef WRITEWORKER_H
#define WRITEWORKER_H
#include "send_worker.h"

static char test_data[WRITE_ONCE_BYTE_SIZE] __attribute__((aligned(WRITE_ONCE_BYTE_SIZE))) = {'a'};

#define likely(x)      __builtin_expect(!!(x), 1)
#define unlikely(x)    __builtin_expect(!!(x), 0)

void direct_io(int index);
void buffer_io(int index);
void mmap_io(int index);

#endif