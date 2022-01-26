#include "../cn.h"

char buff[BUFFSIZE];

void* capitalize_thread(void *fd) {
    
    int nsfd = *(int *)fd;
    if(recv(nsfd,buff,BUFFSIZE,0) < 0)
        perror("recv error");
    capitalize(buff);
    if(send(nsfd,buff,BUFFSIZE,0) < 0)
    perror("send error");

    pthread_exit(NULL);

}


int main() {

    int sfd, nsfd[3];
    socklen_t len;
    struct sockaddr_in server_addr,client_addr;
    pthread_t thread[3];

    init_server_address(&server_addr,LOCAL_HOST,PORT);

    if((sfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
        error("socket error");
    
    if(bind(sfd,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0)
        error("bind error");

    if(listen(sfd,BACKLOG) < 0) 
        error("listen error");
    
    for(int i = 0; i < 3; i++) {
        
        if((nsfd[i] = accept(sfd,(struct sockaddr *)&client_addr,&len)) < 0)
            error("accept error");
        
        cout<<"\nconnection established ..."<<endl;

        pthread_create(&thread[i],NULL,capitalize_thread,&nsfd[i]);
    
    }

    for(int i = 0; i < 3; i++)
        pthread_join(thread[i],NULL);
    
}