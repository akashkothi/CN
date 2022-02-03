#include "../cn.h"
#define MAX_SFDS 3

int used_sfds = 0;
pthread_t sfd_thread[MAX_SFDS];
struct pollfd sfd[MAX_SFDS];
string service[MAX_SFDS] = {"./echo.exe","./capitalize.exe","./de_capitalize.exe"};


void fork_child(const char* path, int nsfd) {

    if(fork() == 0) {

        for(int i = 0; i < used_sfds; i++)
            close(sfd[i].fd);
        
        dup2(nsfd,STDIN_FILENO);
        dup2(nsfd,STDOUT_FILENO);
        
        execl(path,path,NULL);    
    }
}


void* add_service(void* arg) {

    struct sockaddr_in server_addr;
    socklen_t len = sizeof(server_addr);

    init_socket_address(&server_addr,LOCAL_HOST,ports[used_sfds]);
        
    if((sfd[used_sfds].fd = socket(AF_INET,SOCK_STREAM,0)) < 0)
        error("socket error");
        
    if(bind(sfd[used_sfds].fd,(struct sockaddr *)&server_addr,len) < 0)
        error("bind error");

    if(listen(sfd[used_sfds].fd,BACKLOG) < 0) 
        error("listen error");
    
    sfd[used_sfds++].events = POLLRDNORM;

    cout<<"New service added ..."<<endl;

    pthread_exit(NULL);
}

void handler(int signo) {
    pthread_create(&sfd_thread[used_sfds],NULL,add_service,NULL);
}

int main() {

    int nsfd;
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);
    signal(SIGUSR1,handler);

    while(1) {

        if(poll(sfd,used_sfds,500) < 0) {
            if(errno == EINTR)
                continue;
            error("poll error");
        }
        else {

            for(int i = 0; i < used_sfds; i++) {
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