#include "../cn.h"

int main() {

    int usfd, nusfd;
    socklen_t userv_len, ucli_len;
    struct sockaddr_un userv_addr, ucli_addr;
    userv_len = ucli_len = sizeof(struct sockaddr_un);

    int fd = open("./text.txt",R);
    
    unlink(ADDRESS);
    usfd = uds_socket(SOCK_STREAM,ADDRESS,userv_addr);

    if(bind(usfd,(struct sockaddr*)&userv_addr,userv_len) < 0)
        error("bind error");

    listen(usfd,BACKLOG);

    if((nusfd = accept(usfd,(struct sockaddr*)&ucli_addr,&ucli_len)) < 0)
        error("accept error");

    char c;
    int n = 1;

    do {

        for(int i = 0; i < 21 && n; i++) {
            if((n = read(fd,&c,1)) < 0)
                error("read error");
            cout<<c;
        }
        cout<<endl;

        if(send_fd(nusfd,fd) < 0)
            error("send_fd error");
        
    } while(n && (fd = recv_fd(nusfd)));

    shutdown(nusfd,SHUT_RDWR);

}