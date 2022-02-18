#include "../cn.h"

int main() {

    int fd, pid;
    char buff[BUFFSIZE] = {'\0'};
    
    fd = fileno(popen("pidof ./server.exe","r"));
    read(fd,buff,BUFFSIZE);
    pid = atoi(buff);
    kill(pid,SIGUSR1);

    cout<<"Signal sent ..."<<endl;

}