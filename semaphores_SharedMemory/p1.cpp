#include "../cn.h"

int main() {
    char ch;
    sem_unlink("s1");
    sem_unlink("s2");
    sem_t *s1 = sem_open("s1",O_CREAT|O_EXCL,RWX,0);
    sem_t *s2 = sem_open("s2",O_CREAT|O_EXCL,RWX,0);
    
    key_t key_x = ftok("X",1);
    key_t key_y = ftok("Y",2);
    int shmid_x = shmget(key_x,BUFFSIZE,RWX|IPC_CREAT);
    int shmid_y = shmget(key_y,BUFFSIZE,RWX|IPC_CREAT);
    
    int *x = (int*) shmat(shmid_x,0,0);
    int *y = (int*) shmat(shmid_y,0,0);
    
    *x = *y = 1;

    while(1) {
        cout<<"I am reading from shm Y ..."<<endl;
        cout<<"Value of y read from shared memory Y : "<<*y<<endl;
        *x  = *y + 1;
        cout<<"Value of x written to shared memory X : "<<*x<<endl;
        cout<<"Enter any char to signal S1 : ";
        cin>>ch;
        sem_post(s1);
        cout<<"I am waiting for S2 ..."<<endl;
        sem_wait(s2);
    }

    shmdt(x);
    shmdt(y);

}