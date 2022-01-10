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
int fd[2];

void* read_(void *arg) {
    while(1) {
        read(fd[0],r_buff, BUFFSIZE);
        cout<<"Message from parent : "<<r_buff<<endl;
    }
}

void* write_(void *arg) {
    while(1) {
        cin.getline(w_buff, BUFFSIZE);
        write(fd[1], w_buff, BUFFSIZE);       
    }
}

int main() {
    
    pthread_t reader, writer;

    fd[0] = dup(STDIN_FILENO);
    fd[1] = dup(STDOUT_FILENO);

    freopen("/dev/tty", "r", stdin);
    freopen("/dev/tty", "w", stdout);

    pthread_create(&reader,NULL,read_,NULL);
    pthread_create(&writer,NULL,write_,NULL);
    pthread_join(reader,NULL);
    pthread_join(writer,NULL);

}