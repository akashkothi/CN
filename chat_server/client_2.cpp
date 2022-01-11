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

int fd[2];
char r_buff[BUFFSIZE];
string w_buff;

void* read_(void *arg) {
    while(1) {
        read(fd[0],r_buff,BUFFSIZE);
        cout<<r_buff<<endl;
    }
}

void* write_(void *arg) {
    while(1) {
        getline(cin,w_buff);
        w_buff = "@C2 : " + w_buff + '\0';
        write(fd[1],w_buff.c_str(),w_buff.size());      
    }
}

int main() {

    pthread_t reader, writer;

    mkfifo("server_fifo",RWX);
    mkfifo("fifo_2",RWX);
    
    fd[1] = open("server_fifo",W);
    fd[0] = open("fifo_2",R);

    pthread_create(&reader,NULL,read_,NULL);
    pthread_create(&writer,NULL,write_,NULL);
    pthread_join(reader,NULL);
    pthread_join(writer,NULL);

}