#include "../cn.h"
sem_t *s,*l;
char buffer[BUFFSIZE];
char** arg;
int main() {

    sem_unlink("s");
    sem_unlink("l");

    pid_t pid;
    s = sem_open("s",O_CREAT|O_EXCL,RWX,0);
    l = sem_open("l",O_CREAT|O_EXCL,RWX,1);
    int fd = open("text.txt",O_RDONLY,RWX);
    dup2(fd,0);

    if((pid = fork()) < 0)
        cout<<"fork error"<<endl;
    else if(pid == 0)
        execv("./dup2_p2.exe",arg);
    else {
        
        while(1) {
            sem_wait(s);
            if(read(fd,buffer,5))
                cout<<"Parent reading : "<<buffer<<endl;
            else {
                sem_post(l);
                break;
            }
            sem_post(l);
        }
        wait(NULL);

    }
    sem_unlink("s");
    sem_unlink("l");
 
    return 0;
}