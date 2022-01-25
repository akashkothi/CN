#include "../cn.h"

void handler(int signo) {
    cout<<"I got signal from child "<<signo<<endl;
}

int main(){

    pid_t pid;
    
    string ppid = to_string(getpid());

    if((pid = fork()) < 0)
        perror("fork error");
    else if(pid > 0) {
        sleep(1);
        if(kill(pid,SIGUSR1) < 0)
            perror("kill error"); 
        signal(SIGUSR2,handler);
        wait(NULL);
    }
    else {
        execl("./p2_exec.exe","./p2_exec.exe",ppid.c_str(),NULL);
    }

}