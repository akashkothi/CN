#include "../cn.h"

char buff[BUFFSIZE];

int main(int argc, const char* argv[]) {

    if(argc != 2) {

        printf("invalid no.of arguments ...\n");
        printf("1st argument -> ./client.exe \n");
        printf("2nd argument -> port number\n");
        printf("8081 -> D1\n");
        printf("8082 -> D2\n");
        printf("8083 -> D3\n");

        exit(EXIT_FAILURE);
    }

    int sfd, port = atoi(argv[1]);
    struct sockaddr_in server_addr,client_addr;
    socklen_t len = sizeof(client_addr);

    init_socket_address(&server_addr,LOCAL_HOST,port);
    
    if((sfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
        error("socket error");

    if(connect(sfd,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0)
        error("connect error");

    cout<<"connection established ...\n"; 

    while(1) {
        if(recv(sfd,buff,BUFFSIZE,0) < 0)
            error("recv error");    
        cout<<buff<<endl;
    }

}