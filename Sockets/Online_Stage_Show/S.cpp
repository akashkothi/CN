#include "../cn.h"

int main() {

    unlink("mysocket");
    sem_unlink("score1");
    sem_unlink("score2");
    sem_unlink("score3");


    cout<<"\n   --------------SCREEN---------------\n"<<endl;

    int shmid_path;
    key_t shmkey_path;

    if((shmkey_path = ftok("UDS",50)) < 0)
        error("ftok error");

    shmid_path = shmget(shmkey_path,9,0666|IPC_CREAT);

    char *path = (char *) shmat(shmid_path,NULL,0);

    strcpy(path,"mysocket");

    *(path+8) = '\0';
    
    int usfd;
    struct sockaddr_un userv_addr,ucli_addr;
    socklen_t userv_len = sizeof(struct sockaddr_un);

    if((usfd = socket(AF_UNIX,SOCK_STREAM,0)) < 0)
        error("socket error");
    
    bzero(&userv_addr,sizeof(userv_addr));
    userv_addr.sun_family = AF_UNIX;
    strcpy(userv_addr.sun_path, "mysocket");
    
    if(bind(usfd,(struct sockaddr*)&userv_addr,userv_len) < 0)
        error("bind error");
    
    if(listen(usfd,10) < 0)
        error("listen error");

    sem_t *s1 = sem_open("score1",O_CREAT|O_EXCL,0666,0);
    sem_t *s2 = sem_open("score2",O_CREAT|O_EXCL,0666,0);
    sem_t *s3 = sem_open("score3",O_CREAT|O_EXCL,0666,0);

    int sfd;
    socklen_t len = sizeof(struct sockaddr_in);
    struct sockaddr_in screen_addr, performer_addr;

    init_socket_address(&screen_addr,LOCAL_HOST,PORT);

    if((sfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
        error("socket error");
        
    if(bind(sfd,(struct sockaddr *)&screen_addr,len) < 0)
        error("bind error");
    
    if(listen(sfd,10) < 0) 
        error("listen error");

    while(1) {
        
        int nsfd;
        char buff[1024] = {'\0'};

        if((nsfd = accept(sfd,(struct sockaddr*)&performer_addr,&len)) < 0)
            error("accept error"); 
        
        cout<<"Artist's performance started ..."<<endl;

        while(recv(nsfd,buff,1024,0) > 1){
            cout<<buff<<endl;
            for(int i = 0; i < 1024; i++)
                buff[i] = '\0';
        }
        
        cout<<"Artist's performance completed ..."<<endl;
        
        pid_t pidofA = getpid_by_name("./A.exe");

        kill(pidofA,SIGUSR1);

        int nusfd; 

        if((nusfd = accept(usfd,(struct sockaddr*)&ucli_addr,&userv_len)) < 0)
            error("accept error");

        if(send_fd(nusfd,nsfd) < 0)
            error("send_fd error");
        
        cout<<"Waiting for performer's answer ..."<<endl;
        
        nsfd = recv_fd(nusfd);

        if(recv(nsfd,buff,1024,0) < 0)
            error("recv error");
        
        cout<<"Performer\'s answer : "<<buff<<endl;

        sem_post(s1);
        sem_post(s2);
        sem_post(s3);

        cout<<"Performance ended ..."<<endl;

    }

}