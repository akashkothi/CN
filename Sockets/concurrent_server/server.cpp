#include "../cn.h"

char buff[BUFFSIZE];

int main() {

    int sfd, nsfd;
    struct sockaddr_in server_addr,client_addr;
    socklen_t len = sizeof(client_addr);

    init_socket_address(&server_addr,LOCAL_HOST,PORT);

    if((sfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
        error("socket error");
    
    if(bind(sfd,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0)
        error("bind error");

    if(listen(sfd,BACKLOG) < 0) 
        error("listen error");
    
    for(int i = 0; i < 3; i++) {
        
        if((nsfd = accept(sfd,(struct sockaddr *)&client_addr,&len)) < 0)
            error("accept error");
        
        cout<<"\nconnection established ..."<<endl;

        if(fork() == 0) {

            close(sfd);
            
            if(send(nsfd,"Hello I am server",BUFFSIZE,0) < 0)
                error("send error");

            cout<<"P"<<i+1<<" : message sent ..."<<endl;

            if(recv(nsfd,buff,BUFFSIZE,0) < 0)
                error("recv error");

            cout<<"P"<<i+1<<" : message received ..."<<endl;
            cout<<buff<<endl;

            exit(EXIT_SUCCESS);

        }
        
    }

    for(int i = 0; i < 3; i++)
        wait(NULL); 
    
}