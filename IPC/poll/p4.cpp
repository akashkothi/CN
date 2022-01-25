#include "../cn.h"

char buff[BUFFSIZE];

int main() {

    int fd;

    mkfifo("fifo4",RWX);
    fd = open("fifo4",W);

    while(1) {
        cin.getline(buff,BUFFSIZE);
        write(fd,buff,BUFFSIZE);
    }
    

}