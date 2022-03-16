#include "../cn.h"

int main() {

    string info = "Document_3";

    int usfd, rsfd, nsfd;
    struct sockaddr_un uds_server_addr;
    struct sockaddr_in raw_client_addr;
    socklen_t len = sizeof(struct sockaddr_un);
    

    usfd = uds_socket(SOCK_STREAM,"mysocket3",uds_server_addr);

    init_socket_address(&raw_client_addr,LOCAL_HOST);

    if(connect(usfd,(struct sockaddr *)&uds_server_addr,len) < 0)
        error("connect error");

    if((rsfd = recv_fd(usfd)) < 0)
        error("recv_fd error");
    
    if(sendto(rsfd,info.c_str(),info.size(),0,(struct sockaddr*)&raw_client_addr,sizeof(raw_client_addr)) < 0)
        error("sendto error");
    
    if(send_fd(usfd,rsfd) < 0)
        error("send_fd error");

    while(1) {

        if((nsfd = recv_fd(usfd)) < 0)
            error("recv_fd error");
        
        char document[BUFFSIZE] = {'\0'};
        
        if(recv(nsfd,document,BUFFSIZE,0) < 0)
            error("recv error");
        
        cout<<"\nDocument received ..."<<endl;
        cout<<"Verifying the document ..."<<endl;
        cout<<"Verification completed ...\n"<<endl;

        if(send_fd(usfd,nsfd,"Document_3 verified") < 0)
            error("send_fd error");
    }
}