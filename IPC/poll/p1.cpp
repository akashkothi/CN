#include "../cn.h"

char buff[BUFFSIZE];

int main() {

    struct pollfd fd[3];
    mkfifo("fifo2",RWX);
    mkfifo("fifo3",RWX);
    mkfifo("fifo4",RWX);

    fd[0].fd = open("fifo2",R);
    fd[1].fd = open("fifo3",R);
    fd[2].fd = open("fifo4",R);

    for(int i = 0; i < 3; i++) 
        fd[i].events = POLLRDNORM;

    while(1) {

        if(poll(fd,3,500) < 0)
            perror("poll error");
        else {
            for(int i = 0; i < 3; i++){
                if(fd[i].revents == POLLRDNORM ) {

                    if(read(fd[i].fd,buff,BUFFSIZE) < 0)
                        perror("read error");
                    
                    switch(i) {
                        case 0:
                            cout<<"P2 : "<<buff<<endl;
                            break;

                        case 1:
                            cout<<"P3 : "<<buff<<endl;
                            break;

                        case 2:
                            cout<<"P4 : "<<buff<<endl;
                            break;

                        default:
                            break;
                    }
                }
            }
        
        }

    }

}