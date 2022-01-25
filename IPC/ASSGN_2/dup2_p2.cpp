#include "../cn.h"
sem_t *s,*l;
char buffer[BUFFSIZE];

int main(){

    s = sem_open("s",0);
    l = sem_open("l",0);

    // if(s == SEM_FAILED){
    //     perror("sem_open failed");
    //     exit(EXIT_FAILURE);
    // }

    while(1) {
        sem_wait(l);
        if(read(0,buffer,5))
            cout<<"Child reading : "<<buffer<<endl;
        else {
            sem_post(s);
            break;
        }
        sem_post(s);
    }

    sem_unlink("s");
    sem_unlink("l");

    return 0;
}