#include "../cn.h"

const char* name[2] = {"mysocket1","mysocket2"};

int main() {

    struct pollfd sfd[2];
    int nsfd, usfd[2], nusfd[2];
    struct sockaddr_in server_addr[2],client_addr;
    struct sockaddr_un userv_addr[2], ucli_addr[2];
    socklen_t len = sizeof(client_addr);
    socklen_t ucli_len[2], userv_len[2];

    for(int i = 0; i < 2; i++) {

        sfd[i].events = POLLRDNORM;
        
        init_socket_address(&server_addr[i],LOCAL_HOST,ports[i]);
        
        if((sfd[i].fd = socket(AF_INET,SOCK_STREAM,0)) < 0)
            error("socket error");
        
        if(bind(sfd[i].fd,(struct sockaddr *)&server_addr[i],len) < 0)
            error("bind error");

        if(listen(sfd[i].fd,BACKLOG) < 0) 
            error("listen error");
    }


    for(int i = 0; i < 2; i++) {
    
        unlink(name[i]);
        ucli_len[i] = userv_len[i] = sizeof(struct sockaddr_un);

        usfd[i] = uds_socket(SOCK_STREAM,name[i],userv_addr[i]);

        if(bind(usfd[i],(struct sockaddr*)&userv_addr[i],userv_len[i]) < 0)
            error("bind error");

        listen(usfd[i],BACKLOG);
        
        if((nusfd[i] = accept(usfd[i],(struct sockaddr*)&ucli_addr[i],&ucli_len[i])) < 0)
            error("accept error");
    }

    while(1) {

        if(poll(sfd,2,500) < 0)
            error("poll error");
        else {

            for(int i = 0; i < 2; i++) {
                if(sfd[i].revents == POLLRDNORM) {    
                    if((nsfd = accept(sfd[i].fd,(struct sockaddr*)&client_addr,&len)) < 0)
                        error("accept error");
                    cout<<"connection established ...\n"; 
                    if(send_fd(nusfd[i],nsfd) < 0)
                        error("send_fd error");
                }
            }
        
        }
    }
   
}