#include "../cn.h"

char buff[BUFFSIZE];

int main() {

    int fd;
    
    mkfifo("fifo2",RWX);
    fd = open("fifo2",W);

    while(1) {
        cin.getline(buff,BUFFSIZE);
        write(fd,buff,BUFFSIZE);
    }

}