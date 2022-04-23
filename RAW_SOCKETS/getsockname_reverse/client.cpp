#include "../cn.h"

int main() {

    int sfd;
    struct sockaddr_in server_addr;

    init_socket_address(&server_addr,LOCAL_HOST,8080);

    if((sfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
        error("socket error");
    
    if(connect(sfd,(struct sockaddr*)&server_addr,sizeof(struct sockaddr_in)) < 0)
        error("connect error");
    
    cout<<"Connection established ..."<<endl;

    while(1);

}