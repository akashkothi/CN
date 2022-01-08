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

int main(int argc, const char *argv[]) {
    
    int fd[2];
    fd[0] = dup(STDIN_FILENO);
    fd[1] = dup(STDOUT_FILENO);
    
    freopen("/dev/tty", "r", stdin);
    freopen("/dev/tty", "w", stdout);

    while(1) {
        read(fd[0],buff, BUFFSIZE);
        cout<<"\n----------------- Child --------------------\n\n";
        cout<<"Message received from parent :"<<buff<<endl;
        cout<<"Child writing to parent : ";
        cin.getline(buff,BUFFSIZE);
        write(fd[1],buff, BUFFSIZE);
    }

}