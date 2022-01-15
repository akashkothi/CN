#include "../cn.h"

int main() {
    char ch;
    sem_t *s1 = sem_open("s1",0);
    sem_t *s2 = sem_open("s2",0);
    
    key_t key_x = ftok("X",1);
    key_t key_y = ftok("Y",2);
    int shmid_x = shmget(key_x,BUFFSIZE,RWX|IPC_CREAT);
    int shmid_y = shmget(key_y,BUFFSIZE,RWX|IPC_CREAT);
    
    int *x = (int*) shmat(shmid_x,0,0);
    int *y = (int*) shmat(shmid_y,0,0);

    while(1) {
        cout<<"I am waiting for signal S1 ..."<<endl;
        sem_wait(s1);
        cout<<"I am reading from shm X ..."<<endl;
        cout<<"Value of x read from shared memory X : "<<*x<<endl;
        *y = *x + 1;
        cout<<"value of y written to shared memory Y : "<<*y<<endl;
        cout<<"Enter any char to signal S2 : ";
        cin>>ch;
        sem_post(s2);
    }

    shmdt(x);
    shmdt(y);
    shmctl(shmid_x,IPC_RMID,NULL);
    shmctl(shmid_y,IPC_RMID,NULL);
}