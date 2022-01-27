#include "../cn.h"

char buff[BUFFSIZE];

int main() {

    int sfd, nsfd;
    struct sockaddr_in server_addr,client_addr;
    struct sockaddr_in addr,peer_addr;
    socklen_t len = sizeof(client_addr);

    init_socket_address(&server_addr,LOCAL_HOST,PORT);

    if((sfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
        error("socket error");
    
    if(bind(sfd,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0)
        error("bind error");

    if(getsockname(sfd,(struct sockaddr*)&addr,&len) < 0)
        error("getsockname error");
    
    cout<<"Local IP : "<<inet_ntoa(addr.sin_addr)<<endl;
    cout<<"Local Port : "<<addr.sin_port<<endl;

    if(listen(sfd,BACKLOG) < 0) 
        error("listen error");
    
    while(1) {    

        if((nsfd = accept(sfd,(struct sockaddr*)&client_addr,&len)) < 0)
            error("accept error");

        if(getpeername(nsfd,(struct sockaddr*)&peer_addr,&len) < 0)
            perror("getpeername error");
        
        cout<<"Peer IP : "<<inet_ntoa(peer_addr.sin_addr)<<endl;
        cout<<"Peer Port : "<<peer_addr.sin_port<<endl;

        cout<<"connection established ..."<<endl;

        if(recv(nsfd,buff,BUFFSIZE,0) < 0)
            error("recv error");
        
        cout<<"message received ..."<<endl;
        cout<<buff<<"\n"<<endl;
    }
    
}