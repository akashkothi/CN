#include "../cn.h"

int main() {

    cout<<getpid()<<endl;

    int sfd;
    struct sockaddr_in server_addr;

    if((sfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
        error("socket error");
    
    init_socket_address(&server_addr,LOCAL_HOST,8080);

    if(bind(sfd,(struct sockaddr*)&server_addr,sizeof(struct sockaddr)) < 0)
        error("bind error");
    
    if(listen(sfd,BACKLOG) < 0)
        error("listen error");

    
    while(1);

}