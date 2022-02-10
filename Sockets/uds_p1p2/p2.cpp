#include "../cn.h"

int main() {

    int usfd;
    socklen_t userv_len;
    struct sockaddr_un userv_addr;
    userv_len = sizeof(struct sockaddr_un);

    usfd = uds_socket(SOCK_STREAM,ADDRESS,userv_addr);

    if(connect(usfd,(struct sockaddr *)&userv_addr,userv_len) < 0)
        error("connect error");
    
    int fd, n = 1;
    char c;

    while(n && (fd = recv_fd(usfd))) {
        for(int i = 0; i < 20 && n; i++) {
            if((n = read(fd,&c,1)) < 0)
                error("read error");
            cout<<c;
        }
        cout<<endl;

        if(send_fd(usfd,fd) < 0)
            error("send_fd error");
    }

    shutdown(usfd,SHUT_RDWR);

}