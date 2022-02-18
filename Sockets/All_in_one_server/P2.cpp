#include "../cn.h"

int main() {

    int fd;
    mkfifo("F", 0666);

    fd = open("F",O_WRONLY);

    while(1) {
        char buff[BUFFSIZE] = {'\0'};
        cin.getline(buff,BUFFSIZE);
        
        if(write(fd,buff,BUFFSIZE) < 0)
            error("write error");
    }

}