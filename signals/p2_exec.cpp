#include "../cn.h"

void handler(int signo) {
    cout<<"I got signal from parent "<<signo<<endl;
}

int main(int argc, char *argv[]){

    int ppid = atoi(argv[1]);
    
    signal(SIGUSR1,handler);
    sleep(1);
    if(kill(ppid,SIGUSR2) < 0)
        perror("kill error");

}