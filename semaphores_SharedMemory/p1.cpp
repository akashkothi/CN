#include "../cn.h"

int main() {

    sem_unlink("s1");
    sem_unlink("s2");
    sem_t *s1 = sem_open("s1",O_CREAT|O_EXCL,RWX,0);
    sem_t *s2 = sem_open("s2",O_CREAT|O_EXCL,RWX,1);
    
    key_t key_x = ftok("X",1);
    key_t key_y = ftok("Y",2);
    int shmid_x = shmget(key_x,BUFFSIZE,RWX|IPC_CREAT);
    int shmid_y = shmget(key_y,BUFFSIZE,RWX|IPC_CREAT);
    
    char *x = (char*) shmat(shmid_x,0,0);
    char *y = (char*) shmat(shmid_y,0,0);
    strcpy(y,"C");

    for(int i = 1; i <= 5; i++){
        sem_wait(s2);
        cout<<"P1 : Value of y read from shared memory : "<<y<<endl;
        strcpy(x,y);
        strcat(x,"A");
        cout<<"P1 : value of x written to shared memory : "<<x<<endl;
        sem_post(s1);
    }

    shmdt(x);
    shmdt(y);

}