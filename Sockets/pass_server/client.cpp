#include "../cn.h"

int main(int argc, const char* argv[]) {

    if(argc != 2) {

        printf("invalid no.of arguments ...\n");
        printf("1st argument -> ./client.exe \n");
        printf("2nd argument -> port number\n");
        printf("8081\n");
        printf("8082\n");

        exit(EXIT_FAILURE);
    }

    int sfd, port = atoi(argv[1]);
    struct sockaddr_in server_addr;

    init_socket_address(&server_addr,LOCAL_HOST,port);
    
    if((sfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
        error("socket error");

    if(connect(sfd,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0)
        error("connect error");

    cout<<"connection established with "<<inet_ntoa(server_addr.sin_addr)<<" "<<server_addr.sin_port<<endl; 

    close(sfd);
    // while(1);
    
}