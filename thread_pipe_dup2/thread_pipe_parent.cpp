#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <pthread.h>
#include <ctype.h>
#define BUFFSIZE 1024

using namespace std;

char r_buff[BUFFSIZE];
char w_buff[BUFFSIZE];
int fd1[2], fd2[2];

void* read_(void *arg) {
    while(1){
        read(fd2[0], r_buff, BUFFSIZE);
        cout<<"Message from child : "<<r_buff<<endl;
    }
}

void* write_(void *arg) {
    while(1) {
        cin.getline(w_buff, BUFFSIZE);
        write(fd1[1], w_buff, BUFFSIZE);        
    }
}

int main(){

    pid_t pid;
    pthread_t reader, writer;

    if(pipe(fd1) < 0)
        perror("pipe error");
    if(pipe(fd2) < 0)
        perror("pipe error");

    
    if((pid = fork()) < 0)
        perror("fork error");
    else if(pid > 0) {

        close(fd1[0]);
        close(fd2[1]);

        pthread_create(&reader,NULL,read_,NULL);
        pthread_create(&writer,NULL,write_,NULL);
        pthread_join(reader,NULL);
        pthread_join(writer,NULL);

    }
    else {

        dup2(fd1[0],0);
        dup2(fd2[1],1);
        close(fd1[0]);
        close(fd2[1]);
        close(fd1[1]);
        close(fd2[0]);
        
        execl("./thread_pipe_child.exe","./thread_pipe_child.exe",NULL);
    
    }
}