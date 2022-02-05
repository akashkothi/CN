# include "../cn.h"

int main() {

    struct pollfd sfd[3], pdfd[3];
    int dfd[3], nsfd, active_dfds = 0;
    unordered_map<int,vector<int>> dfd_nsfd;
    
    struct sockaddr_in server_addr[3],client_addr;
    socklen_t len = sizeof(client_addr);

    for(int i = 0; i < 3; i++)
        dfd[i] = -1;

    for(int i = 0; i < 3; i++) {
        init_socket_address(&server_addr[i],LOCAL_HOST,ports[i]);

        if((sfd[i].fd = socket(AF_INET,SOCK_STREAM,0)) < 0)
            error("socket error");

        if(bind(sfd[i].fd,(struct sockaddr *)&server_addr[i],len) < 0)
            error("bind error");

        listen(sfd[i].fd, BACKLOG);
    }

    for(int i = 0; i < 3; i++)
        sfd[i].events = POLLRDNORM;

    while(1) {

        if(poll(sfd,3,500) < 0)
            error("poll error");
        else {

            for(int i = 0; i < 3; i++) {
                if(sfd[i].revents == POLLRDNORM) {

                    if((nsfd = accept(sfd[i].fd,(struct sockaddr*)&client_addr,&len)) < 0)
                        error("accept error");

                    cout<<"connection established ..."<<endl;
                    
                    if(dfd[i] == -1) {
                        string path = "./D"+to_string(i+1)+".exe";
                        dfd[i] = fileno(popen(path.c_str(),"r"));
                        pdfd[active_dfds].fd = dfd[i];
                        pdfd[active_dfds++].events = POLLRDNORM;
                    }

                    dfd_nsfd[dfd[i]].push_back(nsfd);

                }
            }
        }

        if(poll(pdfd,active_dfds,500) < 0)
            error("poll error");
        else {

            for(int i = 0; i < active_dfds; i++) {
                if(pdfd[i].revents == POLLRDNORM) {

                    char buff[BUFFSIZE] = {'\0'};
                    
                    if(read(pdfd[i].fd,buff,BUFFSIZE) < 0)
                        error("read error");
                    
                    for(int fd : dfd_nsfd[pdfd[i].fd]) {
                        if(send(fd,buff,BUFFSIZE,0) < 0)
                            error("send error");                        
                    }
                }
            }

        }
    }

}