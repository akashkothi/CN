#include "../cn.h"

char buff[BUFFSIZE];

int main(int argc, const char* argv[]) {

    if(argc != 2) {

        printf("invalid no.of arguments ...\n");
        printf("1st argument -> ./client.exe \n");
        printf("2nd argument -> port number\n");
        printf("8082 -> Capitalize\n");
        printf("8083 -> De Capitalize\n");

        exit(EXIT_FAILURE);
    }

    int sfd, port = atoi(argv[1]);
    struct sockaddr_in server_addr;

    init_socket_address(&server_addr,LOCAL_HOST,port);
    
    if((sfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
        error("socket error");

    if(connect(sfd,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0)
        error("connect error");

    cout<<"connection established ...\n"; 

    string input;
    cout<<"Enter string : ";
    getline(cin,input);  
    input.append("\n"); 

    if(send(sfd,input.c_str(),BUFFSIZE,0) < 0)
        error("send error");

    cout<<"message sent ...\n";

    if(recv(sfd,buff,BUFFSIZE,0) < 0)
        error("recv error");

    cout<<"Received message : "<<buff<<endl;
}