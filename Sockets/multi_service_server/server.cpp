#include "../cn.h"

struct pollfd sfd[3];

void fork_child(const char* path, int nsfd) {

    if(fork() == 0) {

        for(int i = 0; i < 3; i++)
            close(sfd[i].fd);
        
        dup2(nsfd,STDIN_FILENO);
        dup2(nsfd,STDOUT_FILENO);
        
        execl(path,path,NULL);    
    }
}

void echo(int sfd) {

    struct sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);
    char buff[BUFFSIZE] = {'\0'};

    if(recvfrom(sfd,buff,BUFFSIZE,0,(struct sockaddr*)&client_addr,&len) < 0)
        error("recvfrom error");
    cout<<"message received from "<<inet_ntoa(client_addr.sin_addr)<<" "<<client_addr.sin_port<<endl;

    if(sendto(sfd,buff,BUFFSIZE,0,(struct sockaddr*)&client_addr,len) < 0)
        error("sendto error");
    cout<<"message sent ..."<<endl;
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

    pthread_exit(NULL);
}


int main() {

    int nsfd, th = 0;
    struct sockaddr_in server_addr[3],client_addr;
    socklen_t len = sizeof(client_addr);
    pthread_t thread[10];

    init_socket_address(&server_addr[0],LOCAL_HOST,ports[0]);

    if((sfd[0].fd = socket(AF_INET,SOCK_DGRAM,0)) < 0)
        error("socket error");
        
    if(bind(sfd[0].fd,(struct sockaddr *)&server_addr[0],len) < 0)
        error("bind error");


    for(int i = 1; i < 3; i++) {
        
        init_socket_address(&server_addr[i],LOCAL_HOST,ports[i]);
        
        if((sfd[i].fd = socket(AF_INET,SOCK_STREAM,0)) < 0)
            error("socket error");
        
        if(bind(sfd[i].fd,(struct sockaddr *)&server_addr[i],len) < 0)
            error("bind error");

        if(listen(sfd[i].fd,BACKLOG) < 0) 
            error("listen error");
    }
    

    for(int i = 0; i < 3; i++) 
        sfd[i].events = POLLRDNORM;


    while(1) {

        if(poll(sfd,3,500) < 0)
            error("poll error");
        else {

            for(int i = 0; i < 3; i++) {
                if(sfd[i].revents == POLLRDNORM) {    
                    
                    switch(i) {

                        case 0: {
                            echo(sfd[0].fd);
                            break;
                        }

                        case 1: {
                            if((nsfd = accept(sfd[i].fd,(struct sockaddr*)&client_addr,&len)) < 0)
                                error("accept error");
                            cout<<"connection established ..."<<endl;
                            fork_child("./capitalize.exe",nsfd);                            
                            break;
                        }

                        case 2: {
                            if((nsfd = accept(sfd[i].fd,(struct sockaddr*)&client_addr,&len)) < 0)
                                error("accept error");
                            cout<<"connection established ..."<<endl;
                            pthread_create(&thread[th++],NULL,de_capitalize_thread,&nsfd);    
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