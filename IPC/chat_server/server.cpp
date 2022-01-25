#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <ctype.h>

#define BUFFSIZE 1024
#define RWX 0666
#define R O_RDONLY
#define W O_WRONLY
#define RW O_RDWR

using namespace std;

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