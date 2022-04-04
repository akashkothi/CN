#include "../cn.h"


int main(int argc, const char* argv[]) {

    int sfd;
    struct sockaddr_in server_addr;

    init_socket_address(&server_addr,LOCAL_HOST,PORT);
    
    if((sfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
        error("socket error");

    if(connect(sfd,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0)
        error("connect error");

    cout<<"connection established ...\n";  

    while(1) {
        
        char buff[BUFFSIZE] = {'\0'};       

        if(recv(sfd,buff,BUFFSIZE,0) < 0)
            error("recv error");

        cout<<"message received ..."<<endl;

        sleep(3);
    }

}