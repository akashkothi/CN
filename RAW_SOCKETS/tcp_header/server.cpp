#include "../cn.h"

int main() {

    int sfd, nsfd;
    struct sockaddr_in server_addr,client_addr;
    socklen_t len = sizeof(client_addr);

    init_socket_address(&server_addr,LOCAL_HOST,PORT);

    if((sfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
        error("socket error");
    
    if(bind(sfd,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0)
        error("bind error");

    if(listen(sfd,BACKLOG) < 0) 
        error("listen error");
            
    if((nsfd = accept(sfd,(struct sockaddr *)&client_addr,&len)) < 0)
        error("accept error");

    while(1) {
        if(send(nsfd,"Hello I am Akash !!",27,0) < 0)
            error("send error");
        cout<<"message sent ..."<<endl;
        sleep(3);
    }
    
}