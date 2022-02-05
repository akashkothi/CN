#include "../cn.h"

char buff[BUFFSIZE];

int main() {

    int sfd1, sfd2, nsfd;
    struct sockaddr_in server_addr,client_addr;
    struct sockaddr_in p1_addr,s1_addr;
    socklen_t len = sizeof(client_addr);

    init_socket_address(&server_addr,LOCAL_HOST,PORT);

    if((sfd1 = socket(AF_INET,SOCK_STREAM,0)) < 0)
        error("socket error");
    
    if(bind(sfd1,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0)
        error("bind error");

    if(listen(sfd1,BACKLOG) < 0) 
        error("listen error");
    
        
    if((nsfd = accept(sfd1,(struct sockaddr *)&client_addr,&len)) < 0)
        error("accept error");
        
    if(recv(nsfd,&p1_addr,sizeof(p1_addr),0) < 0)
        error("recv error");

    init_socket_address(&s1_addr,LOCAL_HOST,ports[1]);
    
    if((sfd2 = socket(AF_INET,SOCK_STREAM,0)) < 0)
        error("socket error");
    
    if(connect(sfd2,(struct sockaddr*)&s1_addr,sizeof(s1_addr)) < 0)
        error("connect error");

    if(send(sfd2,&p1_addr,sizeof(p1_addr),0) < 0)
        error("send error");
    cout<<"address sent ..."<<endl;     

}