#include "../cn.h"
sem_t *s,*l;

int main() {

    pid_t pid;
    s = sem_open("s",O_CREAT|O_EXCL,777,0);
    l = sem_open("l",O_CREAT|O_EXCL,777,1);

    if((pid = fork()) < 0)
        cout<<"fork error"<<endl;
    else if(pid == 0) {

        for(int i = 0; i < 10; i += 2) {
            sem_wait(l);
            cout<<"In child : Statement-"<<i+1<<endl;
            cout<<"In child : Statement-"<<i+2<<endl;
            sem_post(s);
        }
        
    }
    else {
        
        for(int i = 0; i < 10; i += 2) {            
            sem_wait(s);
            cout<<"In parent : Statement-"<<i+1<<endl;
            cout<<"In parent : Statement-"<<i+2<<endl;
            sem_post(l);
        }
        wait(NULL);

    }
    sem_unlink("s");
    sem_unlink("l");
    return 0;
}