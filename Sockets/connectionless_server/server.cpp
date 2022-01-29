#include "../cn.h"

int main() {

    int sfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t len = sizeof(client_addr);

    init_socket_address(&server_addr,LOCAL_HOST,PORT);

    if((sfd = socket(AF_INET,SOCK_DGRAM,0)) < 0)
        error("socket error");
    
    if(bind(sfd,(struct sockaddr*)&server_addr,len) < 0)
        error("bind error");

    char buff[BUFFSIZE] = {'\0'};

    if(recvfrom(sfd,buff,BUFFSIZE,0,(struct sockaddr*)&client_addr,&len) < 0)
        error("recvfrom error");
    cout<<"message received from "<<inet_ntoa(client_addr.sin_addr)<<" "<<client_addr.sin_port<<endl;

    cout<<buff<<endl;
    
    cout<<"Enter message : ";
    cin.getline(buff,BUFFSIZE);

    if(sendto(sfd,buff,BUFFSIZE,0,(struct sockaddr*)&client_addr,len) < 0)
        error("sendto error");
    cout<<"message sent ..."<<endl;

}