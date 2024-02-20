#ifndef WRITEWORKER_H
#define WRITEWORKER_H
#include "send_worker.h"

void direct_io(int index);
void buffer_io(int index);
void mmap_io(int index);

#endif