#include "../cn.h"

void* capitalize_thread(void *fd) {
    
    char buff[BUFFSIZE] = {'\0'};
    int nsfd = *(int *)fd;

    while(1) {

        if(recv(nsfd,buff,BUFFSIZE,0) < 0)
            perror("recv error");
        cout<<"message received ...\n";

        capitalize(buff);

        if(send(nsfd,buff,BUFFSIZE,0) < 0)
            perror("send error");
        cout<<"message sent ...\n";
        
    }

    cout<<"connection lost ...\n\n";
    pthread_exit(NULL);

}

int main() {

    int usfd, th = 0;
    socklen_t userv_len;
    struct sockaddr_un userv_addr;
    userv_len = sizeof(struct sockaddr_un);
    pthread_t threads[10];

    usfd = uds_socket(SOCK_STREAM,"mysocket1",userv_addr);

    if(connect(usfd,(struct sockaddr *)&userv_addr,userv_len) < 0)
        error("connect error");

    while(1) {
        int nsfd =  recv_fd(usfd);
        pthread_create(&threads[th++],NULL,capitalize_thread,&nsfd);
        cout<<"nsfd received ..."<<endl;
    }


}