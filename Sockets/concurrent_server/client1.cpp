#include "../cn.h"

char buff[BUFFSIZE];

int main() {

    int sfd;
    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    
    if((sfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
        perror("socket error");

    if(inet_pton(AF_INET,LOCAL_HOST,&server_addr.sin_addr) < 1)
        perror("inet_pton error"); 

    if(connect(sfd,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0)
        perror("connect error");

    cout<<"connection established ...\n"; 

    if(recv(sfd,buff,BUFFSIZE,0) < 0)
        perror("recv error");

    cout<<"message received ...\n";
    cout<<buff<<endl;  

    if(send(sfd,"Hello I am client1",BUFFSIZE,0) < 0)
        perror("send error");

    cout<<"message sent ...\n";

}