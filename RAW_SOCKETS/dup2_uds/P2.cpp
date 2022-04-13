#include "../cn.h"

int main() {

    int usfd, fd;
    char buff[BUFFSIZE] = {'\0'};
    struct sockaddr_un server_addr;
    socklen_t len = sizeof(struct sockaddr_un);

    usfd = uds_socket(ADDRESS,server_addr);

    if(connect(usfd,(struct sockaddr*)&server_addr,len) < 0)
        error("connect error");

    dup2(usfd, 0);
    
    cin>>buff;
    cout<<buff<<endl;

}