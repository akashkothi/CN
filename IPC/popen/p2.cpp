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

int main() {
    FILE *fp = popen("./p3.exe","w");
    int fd = fileno(fp);
    dup2(fd,1);
    string buff;
    while(1) {
        getline(cin,buff);
        cout<<buff<<endl;    
    }    
}