#include "../cn.h"

int nsfd[100];
int s = 0, e = 0;

void handler(int signo) {
    
    cout<<"signal received ..."<<endl;

    if(fork() == 0) {
        dup2(nsfd[s],STDIN_FILENO);
        execl("./E.exe","./E.exe",NULL);
    }

    close(nsfd[s++]);

}


int main() {

    signal(SIGUSR1, handler);

    int fd[2], sfd;
    mkfifo("F", 0666);
    struct pollfd pfd[4], psfd[1];
    struct sockaddr_in server_addr, client_addr;
    socklen_t len = sizeof(client_addr);

    if(pipe(fd) < 0)
        error("pipe error");

    if(fork() == 0) {
        close(fd[0]);
        dup2(fd[1],STDOUT_FILENO);
        execl("./P1.exe","./P1.exe",NULL);
    }

    close(fd[1]);

    pfd[0].fd = STDIN_FILENO;
    pfd[1].fd = fd[0];
    pfd[2].fd = open("F",O_RDONLY);
    pfd[3].fd = fileno(popen("./P3.exe","r"));
    
    for(int i = 0; i < 4; i++)
        pfd[i].events = POLLIN;
    
    init_socket_address(&server_addr,LOCAL_HOST,PORT);
    
    if((sfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
        error("socket error");

    if(bind(sfd,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0)
        error("bind error");

    if(listen(sfd,BACKLOG) < 0) 
        error("listen error");
    
    psfd[0].fd = sfd;
    psfd[0].events = POLLIN;

    while(1) {

        if(poll(pfd,4,500) < 0){
            if(errno == EINTR)
                continue;
            error("poll error");
        }
        else {

            for(int i = 0; i < 4; i++) {
                if(pfd[i].revents == POLLIN) {
                    
                    char buff[BUFFSIZE] = {'\0'};
                    
                    if(read(pfd[i].fd,buff,BUFFSIZE) < 0)
                        error("read error");
                    
                    for(int i = s; i < e; i++) { 
                        if(send(nsfd[i],buff,BUFFSIZE,0) < 0)
                            error("send error");
                    }
                }
            }
        }

        if(poll(psfd,1,500) < 0){
            if(errno == EINTR)
                continue;
            error("poll error");
        }
        else {
            if(psfd[0].revents == POLLIN) {
                if((nsfd[e++] = accept(psfd[0].fd,(struct sockaddr*)&client_addr,&len)) < 0)
                    error("accept error");
                cout<<"connection established ..."<<endl;
            }
        }
    }
    
}