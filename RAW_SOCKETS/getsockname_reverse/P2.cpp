#include "../cn.h"

int main() {
    
    char buff[BUFFSIZE] = {'\0'};

    int pid = getpid_by_port(8080);
    
    cout<<pid<<endl;
    
    int sfd, nsfd;
    struct sockaddr_in client_addr,server_addr;
    socklen_t len = sizeof(struct sockaddr_in);

    sfd = syscall(SYS_pidfd_getfd,pid,3,0);

    while(1);

    if((nsfd = accept(sfd,(struct sockaddr*)&client_addr,&len)) < 0)
        error("accept error");
    
    cout<<"connection established ..."<<endl;





}