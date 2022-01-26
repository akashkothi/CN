#include "../cn.h"

char buff[BUFFSIZE];

string client = "Hello I am client-";


int main(int argc, const char* argv[]) {

    if(argc != 2) {
        printf("invalid no.of arguments ...\n");
        printf("1st argument -> ./client.exe \n");
        printf("2nd argument -> client number\n");
        exit(EXIT_FAILURE);
    }

    int sfd;
    struct sockaddr_in server_addr;

    init_server_address(&server_addr,LOCAL_HOST,PORT);
    
    if((sfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
        error("socket error");

    if(connect(sfd,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0)
        error("connect error");

    cout<<"connection established ...\n";  

    client = client + argv[1];

    if(send(sfd,client.c_str(),BUFFSIZE,0) < 0)
        error("send error");

    cout<<"message sent ...\n";

    if(recv(sfd,buff,BUFFSIZE,0) < 0)
        error("recv error");

    cout<<"message received ...\n";
    cout<<buff<<endl; 
}