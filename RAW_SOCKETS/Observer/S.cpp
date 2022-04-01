#include "../cn.h"

int sfd, size = 0;
pollfd pnsfd[100];

void handler(int signo) {

    int nsfd;
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(struct sockaddr_in);

    if((nsfd = accept(sfd,(struct sockaddr*)&client_addr,&len)) < 0)
        error("accept error");
    
    cout<<"client connected ..."<<endl;
    
    pnsfd[size].fd = nsfd;
    pnsfd[size++].events = POLLIN;

    return;
}

int main(int argc, char* argv[]) {

    srand(time(0));

    if(argc != 2) {
        cout<<"USAGE : S.exe <port number>"<<endl;
        exit(EXIT_FAILURE);
    }

    signal(SIGUSR1, handler);

    int port = atoi(argv[1]);
    struct sockaddr_in server_addr,client_addr;
    socklen_t len = sizeof(struct sockaddr_in);

    int usfd[2];
    struct sockaddr_un uds_addr[2];
    socklen_t uds_len = sizeof(struct sockaddr_un);


    for(int i = 0; i < 2; i++) 
        usfd[i] = uds_socket(path[i], uds_addr[i]);


    if((sfd = socket(AF_INET, SOCK_STREAM,0)) < 0)
        error("socket error");

    init_socket_address(&server_addr,LOCAL_HOST,port);

    if(bind(sfd,(struct sockaddr *)&server_addr,sizeof(struct sockaddr_in)) < 0)
        error("bind error");

    if(listen(sfd,BACKLOG) < 0) 
        error("listen error");


    string pid = to_string(getpid());

    int r = rand()%2;
    
    if(connect(usfd[r],(struct sockaddr*)&uds_addr[r],sizeof(struct sockaddr_un)) < 0)
        error("uds connect error");

    
    send_fd(usfd[r], sfd, pid.c_str());

    cout<<endl<<"Port details sent to one of the co-ordinators ..."<<endl;

    while(1) {

        if(poll(pnsfd,size,500) < 0) {
            if(errno == EINTR)
                continue;
            error("poll error");
        }

        for(int i = 0; i < size; i++) {
            if(pnsfd[i].revents == POLLIN) {
                char buff[BUFFSIZE] = {'\0'};
                if(recv(pnsfd[i].fd,buff,BUFFSIZE,0) < 0)
                    error("recv error");
                cout<<buff<<endl;
            }
        }

    }
}

