#include <thread>
#include "send_worker.h"
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <cstring>
#include <cstdio>
#include <random>
#include "global.h"

void send_msg_worker(int index){
    struct msg_buf send_buf;
    struct msqid_ds q_info;
    std::string filename = "./msgfile/msgfile" + std::to_string(index);
    std::cout<<filename<<std::endl;
    send_buf.msg_type=1; 
    key_t key;
    key = ftok(filename.c_str(), 1);

    if(key<0){
        printf("creat key number failed\n");
    }
    else printf("create key number: %x\n", key);

    int msgid = msgget(key, IPC_CREAT|0777);

    if (msgid==-1){
        printf("create queue error\n");
    }
    else printf("create queue msgid: %d\n",msgid);

    msgctl(msgid, IPC_STAT, &q_info);

    int k = 0;
    std::random_device rd;
    std::default_random_engine random(rd());
    while (k<kWriteCountPerThread*4){
        if (q_info.msg_qnum<=8){
            int i=0;
            char tmp;
            while (i<WRITE_ONCE_BYTE_SIZE){
                tmp = random() % 62;
                if (tmp<10){
                    tmp += '0';
                } 
                else 
                    if (tmp<36) {
                        tmp = tmp-10+'A';
                    }
                    else tmp = tmp-36+'a';
                send_buf.data[i]=tmp;
                i++;
            }
            //printf("send_worker index: %d send\n", index);
            msgsnd(msgid,&send_buf,strlen(send_buf.data),0);
            k++;
        }
    }
}

void get_data(int index, char *str){
    struct msg_buf rcv_buf;
    std::string filename = "./msgfile/msgfile" + std::to_string(index/4);
    key_t key;
    key = ftok(filename.c_str(), 1);
    int msgid = msgget(key, IPC_CREAT|0777);
    if(msgid==-1){
        printf("rcv create queue error\n");
    }
    mtx[index/4].lock();
    msgrcv(msgid,&rcv_buf,sizeof(rcv_buf.data),1,0);
    mtx[index/4].unlock();
    //printf("index %d  msgid %d rcv\n",index,msgid);
    strcpy(str, rcv_buf.data);
}

void del_queue(int index){
    key_t key;
    std::string filename = "./msgfile/msgfile" + std::to_string(index);
    key = ftok(filename.c_str(), 1);
    int msgid = msgget(key, IPC_CREAT|0777);
    msgctl(msgid,IPC_RMID,NULL);
    printf("msgid: %d del\n",msgid);
}