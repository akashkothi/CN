#include "../cn.h"

struct sockaddr_in rsfd_server;

void* broadcast(void *arg) {

    int rsfd = *(int *)arg;

    while(1) {

        char buff[BUFFSIZE] = {'\0'};

        socklen_t len = sizeof(struct sockaddr_in);

        if(recvfrom(rsfd,buff,BUFFSIZE,0,(struct sockaddr *)&rsfd_server,&len) < 0)
            error("recvfrom error");
    
        cout<<buff<<endl<<endl;

    }

}

int main(int argc, char* argv[]) {

    if(argc != 2) {
        cout<<"USAGE : C.exe <protocol>"<<endl;
        exit(EXIT_FAILURE);
    }

    pthread_t thread;
    int rsfd, sfd, port, protocol;
    struct sockaddr_in server_addr;
    
    protocol = atoi(argv[1]);

    if((rsfd = socket(AF_INET,SOCK_RAW,protocol)) < 0)
	    error("socket error");
        
    init_socket_address(&rsfd_server,LOCAL_HOST);

    pthread_create(&thread, NULL, broadcast, &rsfd);

    cout<<"Enter the port number : ";
    cin>>port;

    
    if((sfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
        error("socket error");
    
    init_socket_address(&server_addr,LOCAL_HOST,port);

    if(connect(sfd,(struct sockaddr *)&server_addr,sizeof(struct sockaddr_in)) < 0)
        error("connect error");
    
    cout<<"Connected to the server ..."<<endl;

    while(1) {

        string input;
        getline(cin,input); 
        
        if(send(sfd,input.c_str(),BUFFSIZE,0) < 0)
            error("send error");

    }



}