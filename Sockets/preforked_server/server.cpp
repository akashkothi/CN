#include "../cn.h"

char buff[BUFFSIZE];

int main() {

    pid_t pid;
    int sfd, nsfd;
    struct sockaddr_in server_addr,client_addr;
    socklen_t len = sizeof(client_addr);

    init_socket_address(&server_addr,LOCAL_HOST,PORT);

    if((sfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
        perror("socket error"); 
    
    if(bind(sfd,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0)
        perror("bind error");

    if(listen(sfd,BACKLOG) < 0) 
        perror("listen error");


    if((pid = fork()) < 0)
        perror("fork error");
    else if(pid > 0) {

        wait(NULL); 

        while(1) {
            if((nsfd = accept(sfd,(struct sockaddr *)&client_addr,&len)) < 0)
                perror("accept error");
            cout<<"connection established with parent\n";
        }
    }
    else {
        
        for(int i = 0;i < 7; i++) {
            if((nsfd = accept(sfd,(struct sockaddr *)&client_addr,&len)) < 0)
                perror("accept error");
            cout<<"connection established with child\n";
        }
        exit(EXIT_SUCCESS);
    
    }
    
}