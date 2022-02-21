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
    struct sockaddr_in server_addr,client_addr;
    socklen_t len = sizeof(client_addr);

    init_socket_address(&server_addr,LOCAL_HOST,port);
    
    if((sfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
        error("socket error");

    cout<<"Waiting to connect ..."<<endl;

    if(connect(sfd,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0)
        error("connect error");
    
    getsockname(sfd,(struct sockaddr*)&client_addr,&len);
    
    cout<<"My address "<<inet_ntoa(client_addr.sin_addr)<<" "<<client_addr.sin_port<<endl;

    cout<<"connection established with "<<inet_ntoa(server_addr.sin_addr)<<" "<<server_addr.sin_port<<endl; 

    while(1) {
        string input;
        cout<<"Enter input : ";
        getline(cin,input);
        if(send(sfd,input.c_str(),input.size(),0) < 0)
            error("send error");
        cout<<"message sent ..."<<endl;
    }
    
}