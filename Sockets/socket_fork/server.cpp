#include "../cn.h"

char buff[BUFFSIZE];

int main() {

    pid_t pid;
    int sfd, nsfd;
    socklen_t len;
    struct sockaddr_in server_addr,client_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    
    cout<<"\n--------------- Parent -----------------\n";

    if((sfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
        perror("socket error");

    if(inet_pton(AF_INET,LOCAL_HOST,&server_addr.sin_addr) < 1)
        perror("inet_pton error"); 
    
    if(bind(sfd,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0)
        perror("bind error");

    if(listen(sfd,BACKLOG) < 0) 
        perror("listen error");

    cout<<"waiting for connection ...\n";

    if((nsfd = accept(sfd,(struct sockaddr *)&client_addr,&len)) < 0)
        perror("accept error");
    
    cout<<"connection established ...\n";

    if((pid = fork()) < 0)
        perror("fork error");
    else if(pid > 0) {
        close(nsfd);
        wait(NULL); 
        cout<<"Parent terminated ..."<<endl;   
    }
    else {

        cout<<"\n--------------- Child -----------------\n";

        close(sfd);
    
        if(send(nsfd,"Hello I am server",BUFFSIZE,0) < 0)
        perror("send error");

        cout<<"message sent ...\n";

        if(recv(nsfd,buff,BUFFSIZE,0) < 0)
            perror("recv error");

        cout<<"message received ...\n";
        cout<<buff<<endl;
    
    }



    
}