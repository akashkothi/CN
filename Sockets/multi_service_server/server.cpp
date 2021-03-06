#include "../cn.h"

int sfd[3];
fd_set readset;
int max_sfd = -1;

void fork_child(const char* path, int nsfd) {

    if(fork() == 0) {

        for(int i = 0; i < 3; i++)
            close(sfd[i]);
        
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
    cout<<"\nmessage received from "<<inet_ntoa(client_addr.sin_addr)<<" "<<client_addr.sin_port<<endl;

    if(sendto(sfd,buff,BUFFSIZE,0,(struct sockaddr*)&client_addr,len) < 0)
        error("sendto error");
    cout<<"\nmessage sent ..."<<endl;
}

void* de_capitalize_thread(void *fd) {
    
    char buff[BUFFSIZE] = {'\0'};
    int nsfd = *(int *)fd;
    
    if(recv(nsfd,buff,BUFFSIZE,0) < 0)
        perror("recv error");
    cout<<"\nmessage received ...\n";

    de_capitalize(buff);
    
    if(send(nsfd,buff,BUFFSIZE,0) < 0)
        perror("send error");
    cout<<"\nmessage sent ...\n";

    pthread_exit(NULL);
}


int main() {

    int nsfd, th = 0;
    struct sockaddr_in server_addr[3],client_addr;
    socklen_t len = sizeof(client_addr);
    pthread_t thread[10];

    struct timeval t;
    t.tv_sec = t.tv_usec = 0;

    init_socket_address(&server_addr[0],LOCAL_HOST,ports[0]);

    if((sfd[0] = socket(AF_INET,SOCK_DGRAM,0)) < 0)
        error("socket error");
        
    if(bind(sfd[0],(struct sockaddr *)&server_addr[0],len) < 0)
        error("bind error");

    max_sfd = max(max_sfd,sfd[0]);

    for(int i = 1; i < 3; i++) {
        
        init_socket_address(&server_addr[i],LOCAL_HOST,ports[i]);
        
        if((sfd[i] = socket(AF_INET,SOCK_STREAM,0)) < 0)
            error("socket error");
        
        max_sfd = max(max_sfd,sfd[i]);

        if(bind(sfd[i],(struct sockaddr *)&server_addr[i],len) < 0)
            error("bind error");

        if(listen(sfd[i],BACKLOG) < 0) 
            error("listen error");
    }


    while(1) {

        FD_ZERO(&readset);

        for(int i = 0; i < 3; i++)
            FD_SET(sfd[i],&readset);

        if(select(max_sfd + 1, &readset, NULL, NULL, NULL) < 0)
            error("select error");
        else {

            for(int i = 0; i < 3; i++) {
                if(FD_ISSET(sfd[i],&readset)) {    
                        
                    cout<<"\nrequest detected ..."<<endl;

                    switch(i) {

                        case 0: {
                            echo(sfd[0]);
                            break;
                        }

                        case 1: {
                            if((nsfd = accept(sfd[i],(struct sockaddr*)&client_addr,&len)) < 0)
                                error("accept error");
                            cout<<"\nconnection established ..."<<endl;
                            fork_child("./capitalize.exe",nsfd);                            
                            break;
                        }

                        case 2: {
                            if((nsfd = accept(sfd[i],(struct sockaddr*)&client_addr,&len)) < 0)
                                error("accept error");
                            cout<<"\nconnection established ..."<<endl;
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