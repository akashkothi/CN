#include "./cn.h"

int agent_pid;


int main(int argc, char* argv[]) {

    char location[BUFFSIZE] = {'\0'};

    strcpy(location,argv[1]);

    agent_pid = atoi(argv[2]);

    cout<<"Pick up location(taxi) : "<<location<<endl;

    int usfd, nsfd;
    struct sockaddr_un location_addr;
    socklen_t location_len = sizeof(struct sockaddr_un);

    usfd = uds_socket(SOCK_STREAM,location,location_addr);

    if(connect(usfd,(struct sockaddr *)&location_addr,location_len) < 0)
        error("connect error in taxi");
    
    // cout<<"UDS connected ..."<<endl;

    if((nsfd = recv_fd(usfd)) < 0)
        error("recv_fd error");
    
    cout<<"nsfd received from tourism server ..."<<endl;

    while(1) {

        int bytes;
        char buff[BUFFSIZE] = {'\0'};
            
        if((bytes = recv(nsfd,buff,BUFFSIZE,0)) < 0)
            error("recv error");

        else if(bytes == 0) {

            cout<<"Vehicle breakdown occured(taxi) ..."<<endl;
            kill(agent_pid,SIGUSR1);
            exit(EXIT_FAILURE);
        }
        else cout<<buff<<endl; 

    }

}