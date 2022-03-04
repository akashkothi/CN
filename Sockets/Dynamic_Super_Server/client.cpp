#include "../cn.h"

int main(int argc, const char* argv[]) {

    if(argc != 2) {
        printf("invalid no.of arguments ...\n");
        printf("1st argument -> ./client.exe \n");
        printf("2nd argument -> port number\n");
        printf("8081 8082 8083\n");
        exit(EXIT_FAILURE);
    }

    int sfd, port = atoi(argv[1]);
    struct sockaddr_in server_addr;

    init_socket_address(&server_addr,LOCAL_HOST,port);
    
    if((sfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
        error("socket error");

    if(connect(sfd,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0)
        error("connect error");

    while(1) {

        string input;
        getline(cin,input); 

        if(send(sfd,input.c_str(),BUFFSIZE,0) < 0)
            error("send error");
        cout<<"message sent ..."<<endl;
        
        char buff[BUFFSIZE] = {'\0'};

        if(recv(sfd,buff,BUFFSIZE,0) < 0)
            error("recv error");
        cout<<"Received message : "<<buff<<endl;

    }

}