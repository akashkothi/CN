#include "../cn.h"

unordered_map<int,int> sfd1_count;
unordered_map<int,int> sfd2_count;

string service[2] = {"./capitalize.exe","./decapitalize.exe"};

void display() {

    for(auto it : sfd1_count)
        cout<<"Process "<<it.first<<" sfd1 : "<<it.second<<endl;

    for(auto it : sfd2_count)
        cout<<"Process "<<it.first<<" sfd2 : "<<it.second<<endl;

    return ;
}

void handler1(int signo,siginfo_t *info,void* context) {

    cout<<"signal received ..."<<endl;
    int pid = info->si_pid;
    sfd1_count[pid]++;
    display();

}

void handler2(int signo,siginfo_t *info,void* context) {

    cout<<"signal received ..."<<endl;
    int pid = info->si_pid;
    sfd2_count[pid]++;
    display();
    
}


int main() {

    cout<<"\n   --------------PID : "<<getpid()<<"---------------\n"<<endl;

    int nsfd, opt = 1;
    struct pollfd sfd[2];
    struct sockaddr_in server_addr[2],client_addr;
    socklen_t len = sizeof(client_addr);
    struct sigaction act1,act2;

    sigemptyset(&act1.sa_mask);
    act1.sa_flags = 0;
    act1.sa_flags |= SA_SIGINFO; 
    act1.sa_sigaction = handler1; 

    sigemptyset(&act2.sa_mask);
    act2.sa_flags = 0;
    act2.sa_flags |= SA_SIGINFO; 
    act2.sa_sigaction = handler2; 

    if(sigaction(SIGUSR1,&act1,NULL) < 0)
        perror("sigaction error");
    
    if(sigaction(SIGUSR2,&act2,NULL) < 0)
        perror("sigaction error");
    
    
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