#include "../cn.h"

int sfd, nsfd, s = 1;
struct sockaddr_in server_addr;
socklen_t len = sizeof(server_addr);

void client_handler() {

    int opt = 1;
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);

    cout<<"Hello1"<<endl;

    if(setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR|SO_REUSEPORT, &opt, sizeof(opt)))
        error("setsockopt error");
    
    cout<<"Hello2"<<endl;

    if((nsfd = accept(sfd,(struct sockaddr *)&client_addr,&len)) < 0)
        error("accept error");

    cout<<"connection established with "<<inet_ntoa(client_addr.sin_addr)<<" "<<client_addr.sin_port<<endl;

    return;
}

void handler(int signo) {
    cout<<"Signal received"<<endl;
    client_handler();
    s = 0;
}

int main() {

    int opt = 1;
    signal(SIGUSR1,handler);
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);

    init_socket_address(&server_addr,LOCAL_HOST,ports[0]);

    if((sfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
        error("socket error");
    
    if(setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR|SO_REUSEPORT, &opt, sizeof(opt)))
        error("setsockopt error");
    
    if(bind(sfd,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0)
        error("bind error");

    if(listen(sfd,BACKLOG) < 0) 
        error("listen error");
    
    while(1) {
        
        if((nsfd = accept(sfd,(struct sockaddr *)&client_addr,&len)) < 0)
            error("accept error");

        cout<<"connection established with "<<inet_ntoa(client_addr.sin_addr)<<" "<<client_addr.sin_port<<endl;
        
    }
    
    
}