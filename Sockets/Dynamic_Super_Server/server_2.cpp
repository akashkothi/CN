#include "../cn.h"

void* service(void *arg){
    
    int nsfd = *(int *)arg;

    while(1) {

        char buff[BUFFSIZE] = {'\0'};

        if(recv(nsfd,buff,BUFFSIZE,0) < 0)
            error("recv error");

        capitalize(buff);
    
        if(send(nsfd,buff,BUFFSIZE,0) < 0)
            error("send error");
    }

    pthread_exit(NULL);

}

int main(int argc, const char* argv[]) {
   
    pthread_t thread[100];
    int udp_sfd, sfd, nsfd, c = 0;
    socklen_t len = sizeof(struct sockaddr_in);
    struct sockaddr_in dss_addr, my_addr, client_addr;
    

    init_socket_address(&dss_addr,LOCAL_HOST,PORT);
    init_socket_address(&my_addr,LOCAL_HOST,ports[1]);

    if((udp_sfd = socket(AF_INET,SOCK_DGRAM,0)) < 0)
        error("socket error");
    
    if((sfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
        error("socket error");
    
    if(bind(sfd,(struct sockaddr*)&my_addr,len) < 0)
        error("bind error");

    if(listen(sfd,BACKLOG) < 0)
        error("listen error");

    if(sendto(udp_sfd,&my_addr,sizeof(my_addr),0,(struct sockaddr*)&dss_addr,len) < 0)
        error("sendto error");
    cout<<"connection oriented port address sent to dynamic super server ..."<<endl;

    while(1) {

        if((nsfd = accept(sfd,(struct sockaddr*)&client_addr,&len)) < 0)
            error("accept error");

        cout<<"connection established ...\n";

        pthread_create(&thread[c++],NULL,service,&nsfd);

    }

}