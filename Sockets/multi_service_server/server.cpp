#include "../cn.h"

int main() {

    int sfd, nsfd;
    struct sockaddr_in server_addr,client_addr;
    socklen_t len = sizeof(client_addr);

    init_server_address(&server_addr,LOCAL_HOST,PORT);

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

        int service = 0;

        if(recv(nsfd,&service,sizeof(service),0) < 0)
            error("recv error");

        if(fork() == 0) {
            
            close(sfd);
            dup2(nsfd,STDIN_FILENO);
            dup2(nsfd,STDOUT_FILENO);
            close(nsfd);
            
            switch(service) {
                case 1:
                    execl("./service_1.exe","./service_1.exe",NULL);
                    break;
                case 2:
                    execl("./service_2.exe","./service_2.exe",NULL);
                    break;
                default:
                    execl("./service_1.exe","./service_1.exe",NULL);
                    break;
            }

        }
    }

    for(int i = 0; i < 3; i++)
        wait(NULL); 
    
}