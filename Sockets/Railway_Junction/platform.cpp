#include "../cn.h"

int p;

void* announcement(void* arg) {
    
    int msgqid = *(int *)arg;

    struct mymesg message;

    while(1) {
        if(msgrcv(msgqid,&message,sizeof(struct mymesg),p+1,0) < 0)
            error("msgrcv error");
        cout<<message.text<<endl;
    }

}


struct isfree {
    int platform[3];
};

struct isfree* free_p;

int main(int argc, const char* argv[1]) {

    if(argc != 2) {

        printf("invalid no.of arguments ...\n");
        printf("1st argument -> ./platform.exe \n");
        printf("2nd argument -> platform number\n");
        printf("1 2 3\n");

        exit(EXIT_FAILURE);
    }

    p = atoi(argv[1]) - 1;

    int shmid, msgqid;
    key_t shmkey, msgqkey; 
     
    shmkey = ftok("P",20);
    shmid = shmget(shmkey,sizeof(struct isfree),RWX|IPC_CREAT);
    free_p = (struct isfree*)shmat(shmid,NULL,0);

    msgqkey = ftok("Q",21);

    if((msgqid = msgget(msgqkey,RWX|IPC_CREAT)) < 0)
        perror("msgget error");

    int usfd;
    socklen_t userv_len;
    struct sockaddr_un userv_addr;
    userv_len = sizeof(struct sockaddr_un);
    pthread_t thread;

    usfd = uds_socket(SOCK_STREAM,ADDRESS,userv_addr);

    if(connect(usfd,(struct sockaddr *)&userv_addr,userv_len) < 0)
        error("connect error");
    
    pthread_create(&thread,NULL,announcement,&msgqid);

    cout<<"\n    --------------PLATFORM "<<p+1<<"----------------\n"<<endl;

    while(1) {

        cout<<"waiting for the train ..."<<endl;
        int nsfd = recv_fd(usfd), bytes = 1;
        cout<<"train arrived ..."<<endl;

        while(bytes) {

            char buff[BUFFSIZE] = {'\0'};
            
            if((bytes = recv(nsfd,buff,BUFFSIZE,0)) < 0)
                perror("recv error");
            else if(bytes) cout<<buff<<endl;

        }

        free_p->platform[p] = 1;
        cout<<"train left the platform ..."<<endl;
    }

}