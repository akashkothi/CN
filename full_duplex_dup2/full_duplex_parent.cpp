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
    int fd1[2],fd2[2];
    
    if(pipe(fd1) < 0)
        perror("pipe error");
    if(pipe(fd2) < 0)
        perror("pipe error");

    if((pid = fork()) < 0)
        perror("fork error");
    else if(pid > 0) {
        
        while(1) {    
            cout<<"Parent writing to child : ";
            cin.getline(buff,BUFFSIZE);
            write(fd1[1],buff,BUFFSIZE);
            read(fd2[0],buff,BUFFSIZE);
            cout<<"\n----------------- Parent -------------------\n\n";
            cout<<"Message received from child : "<<buff<<endl;
        }

    }
    else {

        dup2(fd1[0],0);
        dup2(fd2[1],1);
        close(fd1[0]);
        close(fd2[1]);     

        execl("./full_duplex_child.exe","./full_duplex_child.exe",NULL);
    
    }

}