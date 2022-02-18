#include "../cn.h"

void* read_(void* arg) {

    int sfd = *(int *)arg;

    while(1) {

        char buff[BUFFSIZE] = {'\0'};

        if(recv(sfd,buff,BUFFSIZE,0) < 0)
            error("recv error");

        cout<<buff<<endl; 
    }

}


int main(int argc, const char* argv[]) {

    int sfd;
    struct sockaddr_in server_addr;
    pthread_t read_thread;

    init_socket_address(&server_addr,LOCAL_HOST,PORT);
    
    if((sfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
        error("socket error");

    if(connect(sfd,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0)
        error("connect error");

    cout<<"connection established ...\n";  

    pthread_create(&read_thread,NULL,read_,&sfd);

    while(1) {
        
        string buff;
        getline(cin,buff);
        buff.push_back('\n');

        if(send(sfd,buff.c_str(),buff.size(),0) < 0)
            error("send error");
        
        cout<<"message sent ..."<<endl;

    }
}