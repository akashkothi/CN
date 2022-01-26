#include "../cn.h"

char buff[BUFFSIZE];

int main() {

    int sfd;
    struct sockaddr_in server_addr;

    init_server_address(&server_addr,LOCAL_HOST,PORT);
    
    if((sfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
        perror("socket error");

    if(connect(sfd,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0)
        perror("connect error");

    cout<<"connection established ...\n";   

    if(send(sfd,"Hello I am client3",BUFFSIZE,0) < 0)
        perror("send error");

    cout<<"message sent ...\n";

    if(recv(sfd,buff,BUFFSIZE,0) < 0)
        perror("recv error");

    cout<<"message received ...\n";
    cout<<buff<<endl; 
}