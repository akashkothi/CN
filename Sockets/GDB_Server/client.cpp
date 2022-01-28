#include "../cn.h"

int main() {

    int sfd;
    struct sockaddr_in server_addr;

    init_socket_address(&server_addr,LOCAL_HOST,PORT);

    if((sfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
        perror("socket error");

    if(connect(sfd,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0)
        perror("connect error");

    cout<<"connection established ..."<<endl;

    int fd = open("./p.cpp",R);
    char buff[BUFFSIZE] = {'\0'};

    while(read(fd,buff,BUFFSIZE)) {
        if(send(sfd,buff,BUFFSIZE,0) < 0)
            error("recv error");
    }

    shutdown(sfd,SHUT_WR); // client stops writing so that server stops reading the file
    
    cout<<"file sent ...\n";

    if(recv(sfd,buff,BUFFSIZE,0) < 0)
        error("recv error");
    
    cout<<buff<<endl;

    close(sfd);

}