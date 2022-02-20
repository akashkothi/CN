#include "../cn.h"

int main() {

    int nsfd, count = 1, opt = 1;
    struct pollfd sfd[2];
    struct sockaddr_in server_addr[2],client_addr;
    socklen_t len = sizeof(client_addr);

    for(int i = 0; i < 2; i++) {
        
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

    cout<<"My address "<<inet_ntoa(server_addr[0].sin_addr)<<" "<<server_addr[0].sin_port<<endl;
        
    while(1) {
    
        if(poll(sfd,2,500) < 0)
            error("poll error");
        else{
            for(int i = 0; i < 2; i++) {
                if(sfd[i].revents == POLLRDNORM) {
                            
                    switch(i) {

                        case 0: {

                            char buff[BUFFSIZE] = {'\0'};
                            int fd = fileno(popen("pidof ./S1.exe","r"));
                            read(fd,buff,BUFFSIZE);
                            int pid = atoi(buff);
                            kill(pid,SIGUSR1);
                            cout<<"Signal sent ..."<<endl;

                            // if((nsfd = accept(sfd[i].fd,(struct sockaddr *)&client_addr,&len)) < 0)
                            //     error("accept error");

                            // cout<<"connection established with "<<inet_ntoa(client_addr.sin_addr)<<" "<<client_addr.sin_port<<endl;
                            
                            break;
                        }

                        case 1: {
                            
                            break;
                        }

                        default:
                            break;
                    } 
                }
            }
        }
    }
   
}