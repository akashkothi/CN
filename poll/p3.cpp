#include "../cn.h"

char buff[BUFFSIZE];

int main() {

    int fd;
    
    mkfifo("fifo3",RWX);
    fd = open("fifo3",W);

    while(1) {
        cin.getline(buff,BUFFSIZE);
        write(fd,buff,BUFFSIZE);
    }
    

}