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

void capitalize(string &s) {
    if(s.size() == 0) return;
    s[0] = toupper(s[0]);
    for(int i = 1; i < s.length(); i++)
        s[i] = tolower(s[i]);
}

int main() {

    string buff;
    mkfifo("./fifo",RWX);
    int fd = open("./fifo",O_RDWR);

    while(1) {
        getline(cin, buff);
        capitalize(buff);
        buff = "Message from P3 : " + buff + '\0';
        write(fd,buff.c_str(),buff.size());
    }

}