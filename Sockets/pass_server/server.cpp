#include "../cn.h"

int main() {

    int nsfd, count = 1, opt = 1;
    struct pollfd sfd[1];
    struct sockaddr_in server_addr[1],client_addr;
    socklen_t len = sizeof(client_addr);

    for(int i = 0; i < 1; i++) {
        
        init_socket_address(&server_addr[i],LOCAL_HOST,ports[i]);

        if((sfd[i].fd = socket(AF_INET,SOCK_STREAM,0)) < 0)
            error("socket error");
        
        if(setsockopt(sfd[i].fd, SOL_SOCKET, SO_REUSEADDR|SO_REUSEPORT, &opt, sizeof(opt)))
            error("setsockopt error");
    
        if(bind(sfd[i].fd,(struct sockaddr *)&server_addr[i],sizeof(server_addr[i])) < 0)
            error("bind error");

        if(listen(sfd[i].fd,BACKLOG) < 0) 
            error("listen error");
        
        sfd[i].events = POLLRDNORM;
    }

    if(fork() == 0) {
        dup2(sfd[0].fd,STDIN_FILENO);
        execl("./S1.exe","./S1.exe",NULL);
    }

    cout<<"My address "<<inet_ntoa(server_addr[0].sin_addr)<<" "<<server_addr[0].sin_port<<endl;
        
    while(1) {
    
        if(poll(sfd,1,500) < 0)
            error("poll error");
        else {
            for(int i = 0; i < 1; i++) {
                if(sfd[i].revents == POLLRDNORM) {
                    int pid = getpid_by_name("./S1.exe");                    
                    kill(pid,SIGUSR1);
                    cout<<"F : Signal sent ..."<<endl;
                    sleep(1);
                }
            }
        }
    }
   
}