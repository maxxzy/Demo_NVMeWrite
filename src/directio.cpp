#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/stat.h> 
#include <vector>
#include <thread>
#include <fcntl.h>

#define likely(x)      __builtin_expect(!!(x), 1)
#define unlikely(x)    __builtin_expect(!!(x), 0)

#define WRITE_ONCE_BYTE_SIZE 4096 * 4

static char data[WRITE_ONCE_BYTE_SIZE] __attribute__((aligned(WRITE_ONCE_BYTE_SIZE))) = {'a'};
static const uint64_t kWriteCountPerThread = 2 * 2;//1000 * 1000;
static const uint64_t kConcurrency = 1;
static const uint64_t kWriteBytesPerThread  = sizeof(data) * kWriteCountPerThread;
static const uint64_t kTotalWriteBytes = kWriteBytesPerThread * kConcurrency;

uint64_t NowMicros() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return static_cast<uint64_t>(tv.tv_sec) * 1000000 + tv.tv_usec;
}

void writer(int index) {
  std::string fname = "data" + std::to_string(index);

  int fd = ::open(fname.c_str(), O_RDWR | O_CREAT | O_APPEND | O_DIRECT, 0645);

  int ret = posix_fallocate(fd, 0, kWriteBytesPerThread);
  if (ret != 0) { 
    printf("fallocate err %d\n", ret);
  }
  lseek(fd, 0, SEEK_SET);

  for (int32_t i = 0; i < kWriteCountPerThread; i++) {
    ::write(fd, data, WRITE_ONCE_BYTE_SIZE);
  }

  close(fd);
}

int main() {
  uint64_t st, ed;
  st = NowMicros();
  std::vector<std::thread> threads;
  writer(0);
  /*
  for(int i = 0; i < kConcurrency; i++) {
    std::thread worker(writer, i);
    threads.push_back(std::move(worker));
  }
  for (int i = 0; i < kConcurrency; i++) {
    threads[i].join();  
  }*/
  ed = NowMicros();
  printf("time elapsed microsecond(us) %lld, %lld MB/s\n", ed - st, kTotalWriteBytes / (ed - st));
  return 0;
}