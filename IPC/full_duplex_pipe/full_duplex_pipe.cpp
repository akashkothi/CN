#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
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
        
        close(fd1[0]);
        close(fd2[1]);

        cout<<"\n----------------- In Parent --------------------\n\n";

        while(1) {
            cout<<"Parent writing to child : ";
            cin.getline(buff,BUFFSIZE);
            write(fd1[1],buff,BUFFSIZE);
            if(strlen(buff) == 0) {
                cout<<"Parent terminated ..."<<endl;
                exit(0);
            }
            read(fd2[0],buff,BUFFSIZE);
            cout<<"\n----------------- In Parent --------------------\n\n";
            if(strlen(buff) == 0) {
                cout<<"Parent terminated ..."<<endl;
                exit(0);
            }
            cout<<"Message from child : "<<buff<<endl;
        }

    }
    else {
        
        close(fd1[1]);
        close(fd2[0]);

        while(1) {
            read(fd1[0],buff,BUFFSIZE);
            cout<<"\n----------------- In Child ---------------------\n\n";
            if(strlen(buff) == 0) {
                cout<<"Child terminated ..."<<endl;
                exit(0);
            }
            cout<<"Message from parent : "<<buff<<endl;
            cout<<"Child writing to parent : ";
            cin.getline(buff,BUFFSIZE);
            write(fd2[1],buff,BUFFSIZE);
            if(strlen(buff) == 0) {
                cout<<"Child terminated ..."<<endl;
                exit(0);
            }
        }

    }

}