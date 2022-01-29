#include "../cn.h"

int main() {

    int sfd;
    struct sockaddr_in server_addr;
    socklen_t len = sizeof(server_addr);

    init_socket_address(&server_addr,LOCAL_HOST,PORT);

    if((sfd = socket(AF_INET,SOCK_DGRAM,0)) < 0)
        error("socket error");
    
    char buff[BUFFSIZE] = {'\0'};

    cout<<"Enter message : ";

    cin.getline(buff,BUFFSIZE);

    if(sendto(sfd,buff,BUFFSIZE,0,(struct sockaddr*)&server_addr,len) < 0)
        error("sendto error");
    cout<<"message sent ..."<<endl;

    if(recvfrom(sfd,buff,BUFFSIZE,0,(struct sockaddr*)&server_addr,&len) < 0)
        error("recvfrom error");
    cout<<"message received ..."<<endl;

    cout<<buff<<endl;


}