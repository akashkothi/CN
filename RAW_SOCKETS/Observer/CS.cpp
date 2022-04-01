#include "../cn.h"

pollfd psfd[100];
int size = 0, rsfd, type;
struct sockaddr_in service[100], client_addr;

string info = "Available port : ";

void* msgq_observer(void *arg) {

    int msqid = *(int *)arg;

    while(1) { 

        int port, opt = 1;
        struct sockaddr_in client_addr;
        socklen_t len = sizeof(struct sockaddr_in);
        
        if(msgrcv(msqid,&msg,sizeof(msg),type,0) < 0) {
            if(errno == EINTR)
                continue;
            error("msgrcv error");
        }

        port = atoi(msg.text);

        info = info + " " + to_string(port);

        init_socket_address(&service[size],LOCAL_HOST,port);

        if((psfd[size].fd = socket(AF_INET,SOCK_STREAM,0)) < 0)
            error("socket error");
        
        psfd[size].events = POLLIN;
    
        setsockopt(psfd[size].fd,SOL_SOCKET,SO_REUSEADDR|SO_REUSEPORT,&opt,sizeof(socklen_t));

        if(bind(psfd[size].fd,(struct sockaddr *)&service[size],sizeof(struct sockaddr_in)) < 0)
            error("bind error");

        if(listen(psfd[size].fd,BACKLOG) < 0) 
            error("listen error");
        
        size++;

        if(sendto(rsfd,msg.text,BUFFSIZE,0,(struct sockaddr*)&client_addr,len) < 0)
            error("sendto error");
        
        cout<<"Port Info broadcasted ...\n"<<endl;

    }

}

int main(int argc, char* argv[]) {
    
    if(argc != 3) {
        cout<<"USAGE : CS.exe <1 or 2>  <protocol>"<<endl;
        exit(EXIT_FAILURE);
    }

    key_t msqkey;
    int msqid, protocol;

    protocol = atoi(argv[2]);

    if((rsfd = socket(AF_INET,SOCK_RAW,protocol)) < 0)
        error("socket error");
    
    init_socket_address(&client_addr,LOCAL_HOST);

    if((msqkey = ftok("./msg_queue",89)) < 0)
        error("ftok error");
    
    if((msqid = msgget(msqkey,RWX|IPC_CREAT)) < 0)
        error("msgget error");
    
    pthread_t thread;

    pthread_create(&thread,NULL,msgq_observer,&msqid);

    while(1) {

        if(poll(psfd,size,500) < 0) {
            if(errno == EINTR)
                continue;
            error("poll error");
        }
        else {

            for(int i = 0; i < size; i++) {

                if(psfd[i].revents == POLLIN) {
                    
                    // if(sendto(rsfd,info.c_str(),info.size(),0,))

                }

            }


        }

    }



}