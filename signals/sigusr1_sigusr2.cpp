#include "../cn.h"

void handler1(int signo) {
    cout<<"I got signal from parent "<<signo<<endl;
}

void handler2(int signo) {
    cout<<"I got signal from child "<<signo<<endl;
}

int main(){

    pid_t pid;
    
    if((pid = fork()) < 0)
        perror("fork error");
    else if(pid > 0) {
        sleep(1);
        if(kill(pid,SIGUSR1) < 0)
            perror("kill error"); 
        signal(SIGUSR2,handler2);
        wait(NULL);
    }
    else {
        signal(SIGUSR1,handler1);
        sleep(1);
        if(kill(getppid(),SIGUSR2) < 0)
            perror("kill error");
    }

}