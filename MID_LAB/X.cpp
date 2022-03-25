#include "./cn.h"

int main(int argc, char* argv[]) {

    char location[BUFFSIZE] = {'\0'};

    strcpy(location,argv[1]);

    cout<<"Pickup location : "<<location<<endl;

    int usfd, nsfd;
    struct sockaddr_un location_addr;
    socklen_t location_len = sizeof(struct sockaddr_un);

    usfd = uds_socket(SOCK_STREAM,location,location_addr);

    if(connect(usfd,(struct sockaddr *)&location_addr,location_len) < 0)
        error("connect error");
    
    // cout<<"UDS connected ..."<<endl;

    if((nsfd = recv_fd(usfd)) < 0)
        error("recv_fd error");
    
    cout<<"nsfd received from tourism server ..."<<endl;

    while(1);

}