#include "../cn.h"

struct isfree {
    int platform[3];
};

struct isfree* free_p;

#define ANNOUNCEMENT "New train arrived at the station"

int check_isfree() {
    
    int p = -1;
    
    while(1) {
        for(p = 0; p < 3; p++){
            if(free_p->platform[p]){
                free_p->platform[p] = 0;
                return p;
            }
        }
    }

    return p;
}


void announce(int msgqid){

    strcpy(msg.text,ANNOUNCEMENT);
    
    for(int i = 0; i < 3; i++){
        msg.type = i + 1;
        if(msgsnd(msgqid,&msg,sizeof(msg),0) < 0)
            error("msgsnd error");
    }

}

int main() {

    int shmid,msgqid;
    key_t shmkey, msgqkey; 
     
    shmkey = ftok("P",20);
    shmid = shmget(shmkey,sizeof(struct isfree),RWX|IPC_CREAT);
    free_p = (struct isfree*)shmat(shmid,NULL,0);

    for(int i = 0; i < 3; i++)
        free_p->platform[i] = 1;

    msgqkey = ftok("Q",21);

    if((msgqid = msgget(msgqkey,RWX|IPC_CREAT)) < 0)
        perror("msgget error");

    struct pollfd sfd[3];
    int nsfd, usfd, nusfd[3];
    struct sockaddr_in server_addr[3],client_addr;
    struct sockaddr_un userv_addr, ucli_addr[3];
    socklen_t len = sizeof(client_addr);
    socklen_t ucli_len[3], userv_len;

    cout<<"\n   --------------STATION----------------\n"<<endl;

    for(int i = 0; i < 3; i++) {
        
        sfd[i].events = POLLRDNORM;

        init_socket_address(&server_addr[i],LOCAL_HOST,ports[i]);
        
        if((sfd[i].fd = socket(AF_INET,SOCK_STREAM,0)) < 0)
            error("socket error");
        
        if(bind(sfd[i].fd,(struct sockaddr *)&server_addr[i],len) < 0)
            error("bind error");

        if(listen(sfd[i].fd,BACKLOG) < 0) 
            error("listen error");
    }
    
    unlink(ADDRESS);
    userv_len = sizeof(struct sockaddr_un);
    
    usfd = uds_socket(SOCK_STREAM,ADDRESS,userv_addr);
    
    if(bind(usfd,(struct sockaddr*)&userv_addr,userv_len) < 0)
        error("bind error");
    
    listen(usfd,BACKLOG);
    
    for(int i = 0; i < 3; i++) {
        
        ucli_len[i] = sizeof(struct sockaddr_un);

        if((nusfd[i] = accept(usfd,(struct sockaddr*)&ucli_addr[i],&ucli_len[i])) < 0)
            error("accept error");
        
        cout<<"platform"<<i+1<<" added ..."<<endl;
    }

    cout<<endl;

    while(1) {

        if(poll(sfd,3,500) < 0)
            error("poll error");
        else {

            for(int i = 0; i < 3; i++) {
                if(sfd[i].revents == POLLRDNORM) {   

                    cout<<"new train arrived waiting for a platform...\n";
                    
                    int p = check_isfree();

                    cout<<"platform available...\n";

                    if((nsfd = accept(sfd[i].fd,(struct sockaddr*)&client_addr,&len)) < 0)
                        error("accept error"); 

                    announce(msgqid);
                    
                    if(send_fd(nusfd[p],nsfd) < 0)
                        error("send_fd error");

                    cout<<"Train sent to platform p"<<p+1<<endl<<endl;

                }
                
            }
        
        }
    }
   
}