#include "../cn.h"

void* echo_thread(void *fd) {
    
    char buff[BUFFSIZE] = {'\0'};
    int nsfd = *(int *)fd;
    
    if(recv(nsfd,buff,BUFFSIZE,0) < 0)
        perror("recv error");
    cout<<"message received ...\n";
    
    if(send(nsfd,buff,BUFFSIZE,0) < 0)
        perror("send error");
    cout<<"message sent ...\n";

    cout<<"connection lost ...\n\n";
    pthread_exit(NULL);

}


void* capitalize_thread(void *fd) {
    
    char buff[BUFFSIZE] = {'\0'};
    int nsfd = *(int *)fd;

    if(recv(nsfd,buff,BUFFSIZE,0) < 0)
        perror("recv error");
    cout<<"message received ...\n";

    capitalize(buff);
    
    if(send(nsfd,buff,BUFFSIZE,0) < 0)
        perror("send error");
    cout<<"message sent ...\n";

    cout<<"connection lost ...\n\n";
    pthread_exit(NULL);

}

void* de_capitalize_thread(void *fd) {
    
    char buff[BUFFSIZE] = {'\0'};
    int nsfd = *(int *)fd;
    
    if(recv(nsfd,buff,BUFFSIZE,0) < 0)
        perror("recv error");
    cout<<"message received ...\n";

    de_capitalize(buff);
    
    if(send(nsfd,buff,BUFFSIZE,0) < 0)
        perror("send error");
    cout<<"message sent ...\n";

    cout<<"connection lost ...\n\n";
    pthread_exit(NULL);

}

int main() {

    int nsfd, th = 0;
    struct pollfd sfd[3];
    struct sockaddr_in server_addr[3],client_addr;
    socklen_t len = sizeof(client_addr);
    pthread_t thread[10];

    for(int i = 0;i < 3; i++) {
        
        init_socket_address(&server_addr[i],LOCAL_HOST,ports[i]);

        if((sfd[i].fd = socket(AF_INET,SOCK_STREAM,0)) < 0)
            error("socket error");
    
        if(bind(sfd[i].fd,(struct sockaddr *)&server_addr[i],sizeof(server_addr[i])) < 0)
            error("bind error");

        if(listen(sfd[i].fd,BACKLOG) < 0) 
            error("listen error");

    }

    for(int i = 0; i < 3; i++) 
        sfd[i].events = POLLRDNORM;
    
     while(1) {

        int count = 0;
        if((count = poll(sfd,3,500)) < 0)
            error("poll error");
        else {
            for(int i = 0; i < 3; i++) {

                if(sfd[i].revents == POLLRDNORM) {
                    
                    if((nsfd = accept(sfd[i].fd,(struct sockaddr*)&client_addr,&len)) < 0)
                        error("accept error");

                    cout<<"connection established ...\n"; 

                    switch(i) {
                        case 0:
                            pthread_create(&thread[th++],NULL,echo_thread,&nsfd);    
                            break;
                        case 1:
                            pthread_create(&thread[th++],NULL,capitalize_thread,&nsfd);    
                            break;
                        case 2:
                            pthread_create(&thread[th++],NULL,de_capitalize_thread,&nsfd);    
                            break;

                        default:
                            pthread_create(&thread[th++],NULL,echo_thread,&nsfd); 
                            break;
                    } 
                }
            }
        
        }
    }
   
}