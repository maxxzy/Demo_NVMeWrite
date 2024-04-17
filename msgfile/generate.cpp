#include <iostream>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[]){
    for (int index=0;index<128;index++){
        std::string fname = "msgfile" + std::to_string(index);
        int fd = ::open(fname.c_str(), O_NOATIME | O_RDWR | O_CREAT, 0644);
        close(fd);
    }
    return 0;
}