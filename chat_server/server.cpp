#include "../cn.h"

int  fd[4];
char buff[BUFFSIZE];

int main() {

    mkfifo("server_fifo",RWX);
    mkfifo("fifo_1",RWX);
    mkfifo("fifo_2",RWX);
    mkfifo("fifo_3",RWX);

    fd[0] = open("server_fifo",R);
    fd[1] = open("fifo_1",W);
    fd[2] = open("fifo_2",W);
    fd[3] = open("fifo_3",W);

    cout<<"I am Server"<<endl;

    while(1) {
 
        read(fd[0],buff,BUFFSIZE);
        cout<<"Data received ..."<<endl;
        switch(buff[2]) {
            case '1':
                write(fd[2],buff,BUFFSIZE);
                write(fd[3],buff,BUFFSIZE);
                break;
            
            case '2':
                write(fd[1],buff,BUFFSIZE);
                write(fd[3],buff,BUFFSIZE);
                break;
            
            case '3':
                write(fd[1],buff,BUFFSIZE);
                write(fd[2],buff,BUFFSIZE);
                break;
            
            default:
                break;
        }
        cout<<"Data sent ..."<<endl;
    }

}