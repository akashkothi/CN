#include "../cn.h"

void handler(int signo) {

    int usfd;
    struct sockaddr_un address;
    socklen_t userv_len = sizeof(struct sockaddr_un);

    key_t shmkey_path = ftok("UDS",50);
    int shmid_path = shmget(shmkey_path,9,0666|IPC_CREAT);
    
    char *path = (char *) shmat(shmid_path,NULL,0);

    if((usfd = socket(AF_UNIX,SOCK_STREAM,0)) < 0)
        error("socket error");
    
    bzero(&address,sizeof(address));
    address.sun_family = AF_UNIX;
    strcpy(address.sun_path, path);

    if(connect(usfd,(struct sockaddr *)&address,userv_len) < 0)
        error("connect error");

    int nsfd = recv_fd(usfd);

    string ques;
    cout<<"Question : ";
    getline(cin,ques);

    if(send(nsfd,ques.c_str(),ques.size(),0) < 0)
        error("send error");
    
    if(send_fd(usfd,nsfd) < 0)
        error("send_fd error");
    
    close(usfd);

}

int main() {

    cout<<"\n   --------------JUDGE 2---------------\n"<<endl;

    sem_t *s2 = sem_open("score2",0);

    signal(SIGUSR2,handler);

    int shmid_score, shmid;
    key_t shmkey_score, shmkey;

    if((shmkey_score = ftok("./score",51)) < 0)
        error("ftok error");

    shmid_score = shmget(shmkey_score,sizeof(int),0666|IPC_CREAT);

    int *score = (int*) shmat(shmid_score,0,0);

    if((shmkey = ftok("./allgiven",70)) < 0)
        error("ftok error");

    shmid = shmget(shmkey,sizeof(int),0666|IPC_CREAT);

    int *allgiven = (int*) shmat(shmid,0,0);

    while(1) {
        sem_wait(s2);
        
        int give_score;
        cout<<"Score given for the performer : ";
        cin>>give_score;

        *score += give_score;
        *allgiven += 1;

    }

}