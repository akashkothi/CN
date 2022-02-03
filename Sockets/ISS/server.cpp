#include "../cn.h"

struct pollfd sfd[3];

string service[3] = {"./echo.exe","./capitalize.exe","./de_capitalize.exe"};

void fork_child(const char* path, int nsfd) {

    if(fork() == 0) {

        for(int i = 0; i < 3; i++)
            close(sfd[i].fd);
        
        dup2(nsfd,STDIN_FILENO);
        dup2(nsfd,STDOUT_FILENO);
        
        execl(path,path,NULL);    
    }
    
    return;
}


int main() {

    int nsfd;
    struct sockaddr_in server_addr[3],client_addr;
    socklen_t len = sizeof(client_addr);

    for(int i = 0; i < 3; i++) {

        sfd[i].events = POLLRDNORM;
        
        init_socket_address(&server_addr[i],LOCAL_HOST,ports[i]);
        
        if((sfd[i].fd = socket(AF_INET,SOCK_STREAM,0)) < 0)
            error("socket error");
        
        if(bind(sfd[i].fd,(struct sockaddr *)&server_addr[i],len) < 0)
            error("bind error");

        if(listen(sfd[i].fd,BACKLOG) < 0) 
            error("listen error");
    }


    while(1) {

        if(poll(sfd,3,500) < 0)
            error("poll error");
        else {

            for(int i = 0; i < 3; i++) {
                if(sfd[i].revents == POLLRDNORM) {    
                    if((nsfd = accept(sfd[i].fd,(struct sockaddr*)&client_addr,&len)) < 0)
                        error("accept error");
                    cout<<"connection established ...\n"; 

                    fork_child(service[i].c_str(),nsfd);                 
                }
            }
        
        }
    }
   
}