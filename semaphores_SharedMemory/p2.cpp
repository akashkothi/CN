#include "../cn.h"

int main() {

    sem_t *s1 = sem_open("s1",0);
    sem_t *s2 = sem_open("s2",0);
    
    key_t key_x = ftok("X",1);
    key_t key_y = ftok("Y",2);
    int shmid_x = shmget(key_x,BUFFSIZE,RWX|IPC_CREAT);
    int shmid_y = shmget(key_y,BUFFSIZE,RWX|IPC_CREAT);
    
    char *x = (char*) shmat(shmid_x,0,0);
    char *y = (char*) shmat(shmid_y,0,0);

    for(int i = 1; i <= 5; i++) {
        sem_wait(s1);
        cout<<"P2 : Value of x read from shared memory : "<<x<<endl;
        strcpy(y,x);
        strcat(y,"B");
        cout<<"P2 : value of y written to shared memory : "<<y<<endl;
        sem_post(s2);
    }

    shmdt(x);
    shmdt(y);
    shmctl(shmid_x,IPC_RMID,NULL);
    shmctl(shmid_y,IPC_RMID,NULL);
}