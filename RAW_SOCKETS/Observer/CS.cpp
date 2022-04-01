#include "../cn.h"

int main(int argc, char* argv[]) {
    
    if(argc != 3) {
        cout<<"USAGE : CS.exe <0 or 1>  <protocol>"<<endl;
        exit(EXIT_FAILURE);
    }

    unordered_map<int,int> sfd_pid;
    string info = "Available Service Ports :";

    pthread_t thread;
    
    int rsfd;
    int protocol = atoi(argv[2]);

    struct sockaddr_in rsfd_client_addr;

    int size = 0; 
    pollfd psfd[100];


    if((rsfd = socket(AF_INET,SOCK_RAW,protocol)) < 0)
        error("socket error");
    
    init_socket_address(&rsfd_client_addr,LOCAL_HOST);
    
    pollfd pusfd[1];
    struct sockaddr_un uds_addr;

    pusfd[0].events = POLLIN;
    pusfd[0].fd = uds_socket_bind(path[atoi(argv[1])], uds_addr);

    while(1) {

        if(poll(pusfd,1,500) < 0) {
            if(errno == EINTR)
                continue;
            error("poll error");            
        }
        else if(pusfd[0].revents == POLLIN) {
            
            int nusfd, sfd;
            struct sockaddr_un uds_client;
            socklen_t uds_len = sizeof(struct sockaddr_un);

            if((nusfd = accept(pusfd[0].fd,(struct sockaddr*)&uds_client,&uds_len)) < 0)
                error("uds accept error");
            
            char pid[BUFFSIZE] = {'\0'};

            if((sfd = recv_fd(nusfd,pid)) < 0)
                error("recv_fd error");

            struct sockaddr_in client_addr;
            socklen_t client_len = sizeof(struct sockaddr_in);

            if(getsockname(sfd,(struct sockaddr*)&client_addr,&client_len) < 0)
                error("getsockname error");
            
            cout<<"Service Port Received : "<<ntohs(client_addr.sin_port)<<endl;

            info = info + " " + to_string(ntohs(client_addr.sin_port));

            psfd[size].fd = sfd;
            sfd_pid[psfd[size].fd] = atoi(pid);
            psfd[size++].events = POLLIN;

            if(sendto(rsfd,info.c_str(),info.size(),0,(struct sockaddr*)&client_addr,sizeof(struct sockaddr_un)) < 0)
                error("sendto error");
        
            cout<<"Port Info Broadcasted ...\n"<<endl;
    
        }

        int cnt;

        if((cnt = poll(psfd,size,500)) < 0) {
            if(errno == EINTR)
                continue;
            error("poll error");
        }
        else if(cnt > 0) {
            for(int i = 0; i < size; i++) {
                if(psfd[i].revents == POLLIN){
                    kill(sfd_pid[psfd[i].fd], SIGUSR1);
                    cout<<"Signal sent ..."<<endl;
                }
            }
            sleep(1);
        }

    }

}