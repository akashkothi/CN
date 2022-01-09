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
#include <sys/stat.h>
#include <ctype.h>
#define BUFFSIZE 1024

using namespace std;

char buff[BUFFSIZE];

void check() {
    if(strlen(buff) == 0) {
        cout<<"P3 terminated ..."<<endl;
        exit(0);
    }
}

int main() {
    
    int fd[2];
    fd[0] = dup(STDIN_FILENO);
    fd[1] = dup(STDOUT_FILENO);
    
    freopen("/dev/tty", "r", stdin);
    freopen("/dev/tty", "w", stdout);

    while(1) {
        read(fd[0],buff, BUFFSIZE);
        cout<<"\n----------------- In Process P3 --------------------\n\n";
        check();
        cout<<"Message received from P2 : "<<buff<<endl;
        cout<<"P3 writing to P2 : ";
        cin.getline(buff,BUFFSIZE);
        write(fd[1],buff, BUFFSIZE);
        check();
    }


}