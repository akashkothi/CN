#include "../cn.h"

int main(int argc, char* argv[]) {

    if(argc != 2) {
        cout<<"USAGE : S.exe <port number>"<<endl;
        exit(EXIT_FAILURE);
    }

    int msqid;
    key_t msqkey;

    int port = atoi(argv[1]), sfd, opt = 1;
    struct sockaddr_in server_addr, client_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    
    int size = 0;
    pollfd pnsfd[100];

    if((msqkey = ftok("./msg_queue",89)) < 0)
        error("ftok error");
    
    if((msqid = msgget(msqkey,RWX|IPC_CREAT)) < 0)
        error("msgget error");

    init_socket_address(&server_addr,LOCAL_HOST,port);

    setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR|SO_REUSEPORT,&opt,sizeof(socklen_t));

    if(bind(sfd,(struct sockaddr *)&server_addr,sizeof(struct sockaddr_in)) < 0)
        error("bind error");

    if(listen(sfd,BACKLOG) < 0) 
        error("listen error");

    msg.type = 1;
    strcpy(msg.text,argv[1]);
    
    if(msgsnd(msqid,&msg,sizeof(msg),0) < 0)
        error("msgsnd error");
    
    cout<<"Port number sent to CS1 ..."<<endl;

    msg.type = 2;

    if(msgsnd(msqid,&msg,sizeof(msg),0) < 0)
        error("msgsnd error");
    
    cout<<"Port number sent to CS2 ..."<<endl;

    while(1) {

        if(poll(pnsfd,size,500) < 0) {
            if(errno == EINTR)
                continue;
            error("poll error");
        }

        for(int i = 0; i < size; i++) {

            if(pnsfd[i].revents == POLLIN) {
                
                char buff[BUFFSIZE] = {'\0'};

                if(recv(pnsfd[i].fd,buff,BUFFSIZE,0) < 0)
                    error("recv error");
                
                cout<<buff<<endl;

            }
        }

    }







}

