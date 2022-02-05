#include "../cn.h"

int sfd, nsfd;
int s = 1;

void client_handler() {

    struct sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);

    if((nsfd = accept(sfd,(struct sockaddr *)&client_addr,&len)) < 0)
        error("accept error");

    cout<<"connection established with "<<inet_ntoa(client_addr.sin_addr)<<" "<<client_addr.sin_port<<endl;

    return;
}

void handler(int signo) {
    client_handler();
    s = 0;
}

int main() {

    struct sockaddr_in server_addr;
    socklen_t len = sizeof(server_addr);
    
    signal(SIGUSR1,handler);

    cout<<"My pid : "<<getpid()<<endl;

    init_socket_address(&server_addr,LOCAL_HOST,ports[1]);

    if((sfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
        error("socket error");
    
    if(bind(sfd,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0)
        error("bind error");

    if(listen(sfd,BACKLOG) < 0) 
        error("listen error");
    
    while(s);
    
}