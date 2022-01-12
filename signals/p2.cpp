#include "../cn.h"

char buff[BUFFSIZE];

int s = 1;

void handler(int signo) {
    cout<<"I got signal from P1"<<endl;
    cout<<"Signo received : "<<signo<<endl;
    s = 0;
}

int main(int argc, char *argv[]){

    int fd, pid;
    string pid_;

    mkfifo("fifo",RWX);
    fd = open("fifo",O_RDWR);
    pid = getpid();
    pid_ = to_string(pid);
    pid_.push_back('\0');

    cout<<"My Process Id : "<<pid<<endl;

    write(fd,pid_.c_str(),pid_.size());

    while(read(fd,buff,pid_.size()) > 0 && atoi(buff) == pid )
        write(fd,pid_.c_str(),pid_.size());
    
    cout<<"Process Id received from fifo : "<<buff<<endl;
    
    sleep(1);
    signal(SIGUSR1,handler);
    if(kill(atoi(buff),SIGUSR2) < 0)
        perror("kill error");

    while(s);
}
