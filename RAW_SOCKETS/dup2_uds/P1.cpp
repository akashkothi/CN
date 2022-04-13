#include "../cn.h"

int main() {

    int usfd, nusfd;
    string buff = "Hello";
    struct sockaddr_un my_addr, client_addr;
    socklen_t len = sizeof(struct sockaddr_un);

    usfd = uds_socket_bind(ADDRESS,my_addr);

    if((nusfd = accept(usfd,(struct sockaddr*)&client_addr,&len)) < 0)
        error("accept error");
    
    dup2(nusfd,1);

    cout<<buff<<endl;

}