#include "../cn.h"

pid_t pid[3];

void handler1(int signo) {
    int r = rand()%3;
    kill(pid[r],SIGUSR2);
}

int main() {

    srand(time(0));

    cout<<"\n   --------------ANCHOR---------------\n"<<endl;

    pid[0] = getpid_by_name("./J1.exe");
    pid[1] = getpid_by_name("./J2.exe");
    pid[2] = getpid_by_name("./J3.exe");

    signal(SIGUSR1,handler1);

    int shmid_score, shmid;
    key_t shmkey_score, shmkey;

    if((shmkey_score = ftok("./score",51)) < 0)
        error("ftok error");

    if((shmkey = ftok("./allgiven",70)) < 0)
        error("ftok error");
    
    shmid_score = shmget(shmkey_score,sizeof(int),0666|IPC_CREAT);
    shmid = shmget(shmkey,sizeof(int),0666|IPC_CREAT);


    int *score = (int*) shmat(shmid_score,0,0);
    int *allgiven = (int*) shmat(shmid,0,0);
    
    *score = 0;
    *allgiven = 0;

    while(1) {
        while(*allgiven != 3);
        cout<<"Total Score of the Performer : "<<*score<<endl<<endl;
        *score = *allgiven = 0;
    }

}