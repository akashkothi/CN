#include "../cn.h"

int  wfd[3];
struct pollfd rfd[3];
char buff[BUFFSIZE];

int main() {

    mkfifo("c1_write_fifo",RWX);
    mkfifo("c1_read_fifo",RWX);
    mkfifo("c2_write_fifo",RWX);
    mkfifo("c2_read_fifo",RWX);
    mkfifo("c3_write_fifo",RWX);
    mkfifo("c3_read_fifo",RWX);

    rfd[0].fd = open("c1_write_fifo",R);
    rfd[1].fd = open("c2_write_fifo",R);
    rfd[2].fd = open("c3_write_fifo",R);
    
    for(int i = 0; i < 3; i++)
        rfd[i].events = POLLRDNORM;

    wfd[0] = open("c1_read_fifo",W);
    wfd[1] = open("c2_read_fifo",W);
    wfd[2] = open("c3_read_fifo",W);

    while(1) {
        if(poll(rfd,3,500) < 0)
            perror("poll error");
        else {
            for(int i = 0; i < 3; i++) {
                if(rfd[i].revents == POLLRDNORM) {
                    read(rfd[i].fd,buff,BUFFSIZE);
                    write(wfd[(i+1)%3],buff,BUFFSIZE);
                    write(wfd[(i+2)%3],buff,BUFFSIZE);
                }
            }
        }
    }

}