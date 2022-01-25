#include "../cn.h"

int  fd[2];
char buff[BUFFSIZE];

int main() {

    mkfifo("write_fifo",RWX);
    mkfifo("read_fifo",RWX);

    fd[0] = open("read_fifo",R);
    fd[1] = open("write_fifo",W);

    cout<<"I am Server"<<endl;

    while(1) {
        read(fd[0],buff,BUFFSIZE);
        cout<<"Data received"<<endl;
        write(fd[1],buff,BUFFSIZE);
        cout<<"Data sent"<<endl;
    }

}