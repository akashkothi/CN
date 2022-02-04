#include "../cn.h"

string client_id;

void* read_(void *arg) {
    int sfd = *(int *)arg;
    while(1) {
        char buff[BUFFSIZE] = {'\0'};
        if(recv(sfd,buff,BUFFSIZE,0) < 0)
            error("recv error");
        cout<<buff<<endl;
    }
}

void* write_(void *arg) {
    int sfd = *(int *)arg;
    while(1) {
        string buff;
        getline(cin,buff);
        buff = client_id + buff + '\0';
        if(send(sfd,buff.c_str(),BUFFSIZE,0) < 0)
            error("send error");
    }
}

int main(int argc, char* argv[]) {

    if(argc != 2) {
        printf("invalid no.of arguments ...\n");
        printf("1st argument -> ./client.exe \n");
        printf("2nd argument -> client number\n");
        exit(EXIT_FAILURE);
    }

    int sfd;
    struct sockaddr_in server_addr;
    pthread_t reader, writer;

    client_id = "@C"+ to_string(atoi(argv[1])) + " : ";

    init_socket_address(&server_addr,LOCAL_HOST,PORT);
    
    if((sfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
        error("socket error");

    if(connect(sfd,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0)
        error("connect error");

    cout<<"connection established ...\n";

    pthread_create(&reader,NULL,read_,&sfd);
    pthread_create(&writer,NULL,write_,&sfd);
    pthread_join(reader,NULL);
    pthread_join(writer,NULL);
    close(sfd);

}