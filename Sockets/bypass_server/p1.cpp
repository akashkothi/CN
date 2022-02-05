#include "../cn.h"

int main() {

    int sfd1, sfd2, nsfd;
    struct sockaddr_in b_sock,my_addr,client_addr;
    socklen_t len = sizeof(my_addr);

    init_socket_address(&b_sock,LOCAL_HOST,PORT);
    
    if((sfd1 = socket(AF_INET,SOCK_STREAM,0)) < 0)
        error("socket error"); 

    if(connect(sfd1,(struct sockaddr *)&b_sock,sizeof(b_sock)) < 0)
        error("connect error");
    cout<<"connection established with bypass server...\n"; 


    init_socket_address(&my_addr,LOCAL_HOST,ports[0]);
    
    if((sfd2 = socket(AF_INET,SOCK_STREAM,0)) < 0)
        error("socket error");
    
    if(bind(sfd2,(struct sockaddr *)&my_addr,sizeof(my_addr)) < 0)
        error("bind error");

    listen(sfd2,BACKLOG);

    if(send(sfd1,&my_addr,sizeof(my_addr),0) < 0)
        error("send error");
    cout<<"address sent ...\n";


    if((nsfd = accept(sfd2,(struct sockaddr*)&client_addr,&len)) < 0)
        error("accept error");
    cout<<"connection established with S1 server ..."<<endl;

    char buff[BUFFSIZE] = {'\0'};

    if(recv(nsfd,buff,BUFFSIZE,0) < 0)
        error("recv error"); 
    
    cout<<buff<<endl;

    if(send(nsfd,"Hello I am process P1",BUFFSIZE,0) < 0)
        error("send error");

}