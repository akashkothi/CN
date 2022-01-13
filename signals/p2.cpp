#include "../cn.h"

char buff[BUFFSIZE];

int s = 1;

void handler(int signo) {
    cout<<"I got signal from P1"<<endl;
    cout<<"Signo received : "<<signo<<endl;
    s = 0;
}

<<<<<<< HEAD
int main(int argc, char *argv[]){
=======
int main() {
>>>>>>> c09b07a (signal_fifo)

    int fd, pid;
    string pid_;

<<<<<<< HEAD
=======
    signal(SIGUSR1,handler);
>>>>>>> c09b07a (signal_fifo)
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
<<<<<<< HEAD
    
    sleep(1);
    signal(SIGUSR1,handler);
    if(kill(atoi(buff),SIGUSR2) < 0)
        perror("kill error");

    while(s);
    
=======

    if(kill(atoi(buff),SIGUSR2) < 0)
        perror("kill error");
    
    while(s);

    unlink("fifo");

>>>>>>> c09b07a (signal_fifo)
}
