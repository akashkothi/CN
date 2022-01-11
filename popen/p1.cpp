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
string buff;

void* read_(void *arg) {
    while(1) {
        read(fd[0],r_buff, BUFFSIZE);
        cout<<r_buff<<"\n"<<endl;
    }
}

void* write_(void *arg) {
    while(1) {
        getline(cin,buff);
        buff.push_back('\n');
        write(fd[1],buff.c_str(),buff.size());        
    }
}


int main() {

    pthread_t reader, writer;

    mkfifo("./fifo",RWX);
    fd[0] = open("./fifo",RW);

    FILE *fp = popen("./p2.exe","w");
    fd[1] = fileno(fp);
    
    pthread_create(&reader,NULL,read_,NULL);
    pthread_create(&writer,NULL,write_,NULL);
    pthread_join(reader,NULL);
    pthread_join(writer,NULL);

}