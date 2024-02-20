#ifndef SENDWORKER_H
#define SENDWORKER_H
#include <mutex>
#include <sys/ipc.h>
#include <sys/msg.h>

struct msg_buf{
    long msg_type;
    char data[4096];
};

void send_msg_worker(int index);
void del_queue(int index);
void get_data(int index, char *str);

#endif