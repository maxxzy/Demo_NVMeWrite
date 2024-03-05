#include "write_worker.h"
#include "send_worker.h"
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/stat.h> 
#include <vector>
#include <thread>
#include <fcntl.h>
#include "global.h"

void buffer_io(int index, int disk){
    std::string fname = diskPath[disk] + std::to_string(index);
    printf("buffer io %d\n", index);
    char data[WRITE_ONCE_BYTE_SIZE] __attribute__((aligned(WRITE_ONCE_BYTE_SIZE)));

    int fd = ::open(fname.c_str(), O_NOATIME | O_RDWR | O_CREAT, 0644);

    int ret = posix_fallocate(fd, 0, kWriteBytesPerThread);
    if (ret != 0) { 
        printf("fallocate err %d\n", ret);
    }
    lseek(fd, 0, SEEK_SET);   

    for (int32_t i = 0; i < kWriteCountPerThread; i++) {
        get_data(index, data);
        int ret = ::write(fd, data, WRITE_ONCE_BYTE_SIZE);
        if (ret<0){
            printf("duffer_io write error\n");
        }
        else{
            statMtx.lock();
            WriteBytesStat += WRITE_ONCE_BYTE_SIZE;
            statMtx.unlock();
        }
    }

  close(fd);
}

void direct_io(int index, int disk){
    std::string fname = diskPath[disk] + std::to_string(index);
    printf("direct io %d\n", index);
    char data[WRITE_ONCE_BYTE_SIZE] __attribute__((aligned(WRITE_ONCE_BYTE_SIZE)));

    int fd = ::open(fname.c_str(), O_DIRECT | O_NOATIME | O_RDWR | O_CREAT, 0644);

    int ret = posix_fallocate(fd, 0, kWriteBytesPerThread);
    if (ret != 0) { 
        printf("fallocate err %d\n", ret);
    }
    lseek(fd, 0, SEEK_SET);

    for (int32_t i = 0; i < kWriteCountPerThread; i++) {  
        get_data(index, data);
        int ret = ::write(fd, data, WRITE_ONCE_BYTE_SIZE);
        if (ret<0){
            printf("direct_io write error\n");
        }
        else{
            statMtx.lock();
            WriteBytesStat += WRITE_ONCE_BYTE_SIZE;
            statMtx.unlock();
        }
    }

    close(fd);
}

void mmap_io(int index, int disk){
    std::string fname = diskPath[disk] + std::to_string(index);
    std::string staging_fname = "./staging/staging" + std::to_string(index);
    printf("mmap io %d\n", index);
    char data[WRITE_ONCE_BYTE_SIZE] __attribute__((aligned(WRITE_ONCE_BYTE_SIZE)));
    char* base = nullptr;
    char* cursor = nullptr;
    uint64_t staging_offset = 0, file_offset = 0;

    int staging_fd = ::open(staging_fname.c_str(), O_NOATIME | O_RDWR | O_CREAT, 0644);
    int ret = posix_fallocate(staging_fd, 0, kStagingFileSize);                                                                                                                                                                                                              
    if (ret != 0) { 
        printf("fallocate err %d\n", ret);
    }
    void* staging_ptr = mmap(NULL, kStagingFileSize, PROT_READ | PROT_WRITE,
                            MAP_SHARED, staging_fd, 0);
    if (unlikely(staging_ptr == MAP_FAILED))
    {
        printf("mmap err\n");
    }

    int fd = ::open(fname.c_str(), O_DIRECT | O_NOATIME | O_RDWR | O_CREAT, 0644);
    ret = posix_fallocate(fd, 0, kWriteBytesPerThread);
    if (ret != 0) { 
        printf("fallocate err %d\n", ret);
    }
    lseek(fd, 0, SEEK_SET);

    base = static_cast<char*>(staging_ptr);
    cursor = static_cast<char*>(staging_ptr);
    for (int32_t i = 0; i < kWriteCountPerThread; i++) {
        if (unlikely(staging_offset >= kStagingFileSize)) {
            pwrite64(fd, staging_ptr, kStagingFileSize, file_offset);
            cursor = base;
            file_offset += kStagingFileSize;
            staging_offset = 0;
        }
        get_data(index, data);
        memcpy(cursor, data, 4096);
        statMtx.lock();
        WriteBytesStat += WRITE_ONCE_BYTE_SIZE;
        statMtx.unlock();
        cursor += 4096;
        staging_offset += 4096;
    }

    close(staging_fd);
    close(fd);
    munmap(staging_ptr, kStagingFileSize);
    staging_ptr = nullptr;
}