#include "../cn.h"

int main() {

    int rsfd, sfd, nsfd;
    int usfd[3], nusfd[3];
    socklen_t uds_len[3];
    socklen_t tcp_len = sizeof(struct sockaddr_in);
    struct sockaddr_un uds_server_addr[3], uds_client_addr[3];
    struct sockaddr_in tcp_server_addr, tcp_client_addr, raw_client_addr;

    
    if((rsfd = socket(AF_INET,SOCK_RAW,8)) < 0)
	    error("socket error");
    
    init_socket_address(&raw_client_addr,LOCAL_HOST);


    for(int i = 0; i < 3; i++) {
        
        unlink(path[i]);
        
        uds_len[i] = sizeof(struct sockaddr_un);

        usfd[i] = uds_socket(SOCK_STREAM,path[i],uds_server_addr[i]);
        
        if(bind(usfd[i],(struct sockaddr*)&uds_server_addr[i],uds_len[i]) < 0)
            error("bind error");
        
        if(listen(usfd[i],BACKLOG) < 0)
            error("listen error");

    }


    if((sfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
        error("socket error");
    
    init_socket_address(&tcp_server_addr,LOCAL_HOST,PORT);
        
    if(bind(sfd,(struct sockaddr *)&tcp_server_addr,tcp_len) < 0)
        error("bind error");

    if(listen(sfd,BACKLOG) < 0) 
        error("listen error");

    
    for(int i = 0; i < 3; i++) {
        if((nusfd[i] = accept(usfd[i],(struct sockaddr*)&uds_client_addr[i],&uds_len[i])) < 0)
            error("accept error");
    }


    for(int i = 0; i < 3; i++) {

        if(send_fd(nusfd[i],rsfd) < 0)
            error("send_fd error");

        if((rsfd = recv_fd(nusfd[i])) < 0)
            error("recv_fd error");

    }

    while(1) {

        if((nsfd = accept(sfd,(struct sockaddr*)&tcp_client_addr,&tcp_len)) < 0)
            error("accept error");
        
        
        for(int i = 0; i < 3; i++) {

            char opinion[BUFFSIZE] = {'\0'};

            if(send_fd(nusfd[i],nsfd) < 0)
                error("send_fd error");

            if((nsfd = recv_fd(nusfd[i],opinion)) < 0)
                error("recv_fd error");
            
            if(send(nsfd,opinion,BUFFSIZE,0) < 0)
                error("send error");

        }

        cout<<"\nVerification completed ..."<<endl;

    }
   

}