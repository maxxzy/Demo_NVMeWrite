#ifndef WRITEWORKER_H
#define WRITEWORKER_H
#include "send_worker.h"

void direct_io(int index, int disk);
void buffer_io(int index, int disk);
void mmap_io(int index, int disk);

#endif