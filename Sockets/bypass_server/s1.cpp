#include "../cn.h"

char buff[BUFFSIZE];

int main() {

    int sfd1, sfd2, nsfd;
    struct sockaddr_in server_addr,client_addr;
    struct sockaddr_in p1_addr;
    socklen_t len = sizeof(client_addr);

    init_socket_address(&server_addr,LOCAL_HOST,ports[1]);

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

    if((sfd2 = socket(AF_INET,SOCK_STREAM,0)) < 0)
        error("socket error");
    
    if(connect(sfd2,(struct sockaddr*)&p1_addr,sizeof(p1_addr)) < 0)
        error("connect error");
    cout<<"connection established with P1 process"<<endl;

    if(send(sfd2,"Hello I am S1 server",BUFFSIZE,0) < 0)
        error("send error"); 
    
    if(recv(sfd2,buff,BUFFSIZE,0) < 0)
        error("recv error");
    
    cout<<buff<<endl;
}