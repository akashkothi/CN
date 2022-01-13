#include "../cn.h"

int s = 1;

void handler(int signo) {
    cout<<"I got signal from P2"<<endl;
    cout<<"Signo received : "<<signo<<endl;
    s = 0;
}

char buff[BUFFSIZE];

<<<<<<< HEAD
int main(){
=======
int main() {
>>>>>>> c09b07a (signal_fifo)
    
    int fd, pid;
    string pid_;

<<<<<<< HEAD
=======
    signal(SIGUSR2,handler);
>>>>>>> c09b07a (signal_fifo)
    mkfifo("fifo",RWX);
    fd = open("fifo",O_RDWR);
    pid = getpid();
    pid_ = to_string(pid);
    pid_.push_back('\0');

    cout<<"My Process Id : "<<pid<<endl;

    write(fd,pid_.c_str(),pid_.size());

    while(read(fd,buff,pid_.size()) > 0 && atoi(buff) == pid)
        write(fd,pid_.c_str(),pid_.size());

    cout<<"Process Id received from fifo : "<<buff<<endl;
<<<<<<< HEAD

    signal(SIGUSR2,handler);
    sleep(1);
    if(kill(atoi(buff),SIGUSR1) < 0)
        perror("kill error"); 
    
    while(s);

=======
    
    while(s);

    if(kill(atoi(buff),SIGUSR1) < 0)
        perror("kill error"); 

    unlink("fifo");

>>>>>>> c09b07a (signal_fifo)
}