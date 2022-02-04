#include "../cn.h"
#define MAX_CLIENTS 10

int main() {

    fd_set readset1,readset2;
    int nsfd[MAX_CLIENTS], sfd;
    int max_nsfd = -1, clients = 0;
    struct sockaddr_in server_addr,client_addr;
    socklen_t len = sizeof(client_addr);
    struct timeval t;
    t.tv_sec = 0, t.tv_usec = 500;

    init_socket_address(&server_addr,LOCAL_HOST,PORT);

    if((sfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
        error("socket error");
        
    if(bind(sfd,(struct sockaddr *)&server_addr,len) < 0)
        error("bind error");

    listen(sfd,BACKLOG);

    while(1) {

        FD_ZERO(&readset1);
        FD_SET(sfd,&readset1);

        if(select(sfd + 1, &readset1, NULL, NULL, &t) < 0)
            error("select error");
        else {
            if(FD_ISSET(sfd,&readset1)) {
                if((nsfd[clients] = accept(sfd,(struct sockaddr*)&client_addr,&len)) < 0)
                    error("accept error");
                cout<<"\nconnection established with port "<<client_addr.sin_port<<endl;
                max_nsfd = max(max_nsfd,nsfd[clients++]);
            }
        }


        FD_ZERO(&readset2);
        for(int i = 0; i < clients; i++)
            FD_SET(nsfd[i],&readset2);

        if(select(FD_SETSIZE, &readset2, NULL, NULL, &t) < 0)
            error("select error");
        else {
            for(int i = 0; i < clients; i++) {
                if(FD_ISSET(nsfd[i],&readset2)) {
                    
                    char buff[BUFFSIZE] = {'\0'};

                    if(recv(nsfd[i],buff,BUFFSIZE,0) < 0)
                        error("recv error");

                    for(int j = 1; j < clients; j++) {
                        if(send(nsfd[(i+j)%clients],buff,BUFFSIZE,0) < 0)
                            error("send error");
                    }
                    
                }
            }
        }
        
    }
    close(sfd);
}