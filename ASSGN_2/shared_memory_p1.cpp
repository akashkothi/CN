#include "../cn.h"

int main() {

    sem_unlink("x");
    sem_unlink("y");
    sem_t *s1 = sem_open("x",O_CREAT|O_EXCL,RWX,0);
    sem_t *s2 = sem_open("y",O_CREAT|O_EXCL,RWX,1);
    
    key_t key_x = ftok("X",1);
    key_t key_y = ftok("Y",2);
    int shmid_x = shmget(key_x,BUFFSIZE,RWX|IPC_CREAT);
    int shmid_y = shmget(key_y,BUFFSIZE,RWX|IPC_CREAT);
    
    int *x = (int*) shmat(shmid_x,0,0);
    int *y = (int*) shmat(shmid_y,0,0);
    *y = 5;

    for(int i = 1; i <= 5; i++){
        sem_wait(s2);
        cout<<"P1 : Value of y read from shared memory : "<<*y<<endl;
        *x = *y + 1;
        cout<<"P1 : value of x written to shared memory : "<<*x<<endl;
        sem_post(s1);
    }

    shmdt(x);
    shmdt(y);

}