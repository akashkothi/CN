#include "../cn.h"
sem_t *s,*l;
char buffer[BUFFSIZE];

int main() {

    pid_t pid;
    s = sem_open("s",O_CREAT|O_EXCL,777,0);
    l = sem_open("l",O_CREAT|O_EXCL,777,1);

    int fd = open("text.txt",O_RDONLY,777);

    if((pid = fork()) < 0)
        cout<<"fork error"<<endl;
    else if(pid == 0) {

        while(1) {
            sem_wait(l);
            if(read(fd,buffer,5))
                cout<<"Child reading : "<<buffer<<endl;
            else {
                sem_post(s);
                break;
            }
            sem_post(s);
        }
        
    }
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