#include "../cn.h"

int dsfd = dup(STDIN_FILENO);

pthread_t threads[100];
int th = 0;

void* service(void *arg){
    int nsfd = *(int *)arg;

    while(1) {
        char buff[BUFFSIZE] = {'\0'};
        if(recv(nsfd,buff,BUFFSIZE,0) < 0)
            error("recv error");

        cout<<buff<<endl;
    }

}

void handler(int signo) {

    cout<<"S1 : Signal received"<<endl;
    
    int nsfd;
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);

    if((nsfd = accept(dsfd,(struct sockaddr *)&client_addr,&len)) < 0)
        error("accept error");

    cout<<"S1 : connection established with signal "<<inet_ntoa(client_addr.sin_addr)<<" "<<client_addr.sin_port<<endl;

    pthread_create(&threads[th++],NULL,service,&nsfd);

}

int main() {

    signal(SIGUSR1,handler);
    freopen("/dev/tty","r", stdin);
    
    int  nsfd, opt = 1;
    struct pollfd sfd[1];
    struct sockaddr_in server_addr;
    socklen_t len = sizeof(server_addr);

    init_socket_address(&server_addr,LOCAL_HOST,ports[0]);

    if((sfd[0].fd = socket(AF_INET,SOCK_STREAM,0)) < 0)
        error("socket error");
    
    if(setsockopt(sfd[0].fd, SOL_SOCKET, SO_REUSEADDR|SO_REUSEPORT, &opt, sizeof(opt)))
        error("setsockopt error");
    
    if(bind(sfd[0].fd,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0)
        error("bind error");

    if(listen(sfd[0].fd,BACKLOG) < 0) 
        error("listen error");
    
    sfd[0].events = POLLIN;
    
    while(1) {
    
        struct sockaddr_in client_addr;
        socklen_t len = sizeof(client_addr);

        if(poll(sfd,1,500) < 0){
            if(errno == EINTR)
                continue;
            error("poll error");
        }
        else if(sfd[0].revents == POLLIN) {
            
            if((nsfd = accept(sfd[0].fd,(struct sockaddr *)&client_addr,&len)) < 0)
                error("accept error");
            cout<<"S1 : connection established without signal "<<inet_ntoa(client_addr.sin_addr)<<" "<<client_addr.sin_port<<endl;
            
            pthread_create(&threads[th++],NULL,service,&nsfd);
        }
    }
    
}