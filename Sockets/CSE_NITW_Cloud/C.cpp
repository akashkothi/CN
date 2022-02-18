#include "../cn.h"

string service[2] = {"./capitalize.exe","./decapitalize.exe"};

int main() {

    cout<<"\n   --------------PID : "<<getpid()<<"---------------\n"<<endl;

    int nsfd, opt = 1;
    struct pollfd sfd[2];
    struct sockaddr_in server_addr[2],client_addr;
    socklen_t len = sizeof(client_addr);
    struct sigaction act1,act2; 
    
    int fd, pid;
    char buff[BUFFSIZE] = {'\0'};
    
    fd = fileno(popen("pidof ./server.exe","r"));
    read(fd,buff,BUFFSIZE);
    pid = atoi(buff);
    
    for(int i = 0; i < 2; i++){
        
        init_socket_address(&server_addr[i],LOCAL_HOST,ports[i]);

        if((sfd[i].fd = socket(AF_INET,SOCK_STREAM,0)) < 0)
            error("socket error");

        if(setsockopt(sfd[i].fd, SOL_SOCKET, SO_REUSEADDR|SO_REUSEPORT, &opt, sizeof(opt)))
            error("setsockopt error");
    
        if(bind(sfd[i].fd,(struct sockaddr *)&server_addr[i],sizeof(server_addr[i])) < 0)
            error("bind error");

        if(listen(sfd[i].fd,BACKLOG) < 0) 
            error("listen error");
        
        sfd[i].events = POLLIN;

    }

    while(1) {

        if(poll(sfd,2,500) < 0){
            if(errno == EINTR)
                continue;
            error("poll error");
        }
        else {

            for(int i = 0; i < 2; i++) {
                if(sfd[i].revents == POLLIN) {

                    if((nsfd = accept(sfd[i].fd,(struct sockaddr*)&client_addr,&len)) < 0)
                        error("accept error");
                    
                    cout<<"connection established ..."<<endl;

                    if(i == 0)  
                        kill(pid,SIGUSR1);
                    else  kill(pid,SIGUSR2);

                    cout<<"signal sent to server ..."<<endl;

                    if(fork() == 0) {
                        dup2(nsfd,STDIN_FILENO);
                        dup2(nsfd,STDOUT_FILENO);
                        execl(service[i].c_str(),service[i].c_str(),NULL);
                    }

                }
            }
        }
    }

}