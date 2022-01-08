#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <ctype.h>
#define BUFFSIZE 1024

using namespace std;

char buff[BUFFSIZE];

int main() {

    pid_t pid;
    int fd[2];

    if(pipe(fd) < 0)
        perror("pipe error");

    if((pid = fork()) < 0)
        perror("fork error");
    else if(pid > 0) {
        
        close(fd[0]);

        while(1) {
            cout<<"Parent writing to child : ";
            cin.getline(buff,BUFFSIZE);
            write(fd[1],buff,BUFFSIZE);
        }

    }
    else {
        
        close(fd[1]);

        while(1) {
            read(fd[0],buff,BUFFSIZE);
            cout<<"Child reading from parent : "<<buff<<endl;
        }

    }

}