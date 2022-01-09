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
#include <string.h>
#define BUFFSIZE 1024

using namespace std;

char buff[BUFFSIZE];

void check() {
    if(strlen(buff) == 0){
        cout<<"P2 terminated ..."<<endl;
        exit(0);
    }
}

int main() {
    
    int fd[2], fd1[2], fd2[2];
    pid_t pid;

    fd[0] = dup(STDIN_FILENO);
    fd[1] = dup(STDOUT_FILENO);
    
    freopen("/dev/tty", "r", stdin);
    freopen("/dev/tty", "w", stdout);
    
    if(pipe(fd1) < 0)
        perror("pipe error");
    if(pipe(fd2) < 0)
        perror("pipe error");

    if((pid = fork()) < 0)
        perror("fork error");
    else if(pid > 0) {

        close(fd1[0]);
        close(fd2[1]);
        
        while(1) {
            read(fd[0],buff, BUFFSIZE);
            cout<<"\n----------------- In process P2 --------------------\n\n";
            cout<<"Sending message to P3 ..."<<endl;
            write(fd1[1],buff,BUFFSIZE);
            check();
            read(fd2[0],buff,BUFFSIZE);
            cout<<"\n----------------- In process P2 --------------------\n\n";
            cout<<"Sending message to P1 ..."<<endl;
            write(fd[1],buff, BUFFSIZE);
            check();
        }

    }
    else {

        dup2(fd1[0],0);
        dup2(fd2[1],1);
        close(fd1[0]);
        close(fd2[1]);
        close(fd1[1]);
        close(fd2[0]);      

        execl("./p3.exe","./p3.exe",NULL);
    
    }

}