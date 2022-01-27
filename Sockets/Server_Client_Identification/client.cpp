#include "../cn.h"

char buff[BUFFSIZE];

int main(int argc, const char* argv[]) {

    int sfd;
    struct sockaddr_in server_addr;
    struct sockaddr_in addr,peer_addr;
    socklen_t len = sizeof(server_addr);
    string buff;

    init_socket_address(&server_addr,LOCAL_HOST,PORT);
    
    if((sfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
        error("socket error");

    if(connect(sfd,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0)
        error("connect error");

    if(getsockname(sfd,(struct sockaddr*)&addr,&len) < 0)
        error("getsockname error");
    
    cout<<"Local IP : "<<inet_ntoa(addr.sin_addr)<<endl;
    cout<<"Local Port : "<<addr.sin_port<<endl;

    cout<<"connection established ...\n";  

    if(getpeername(sfd,(struct sockaddr*)&peer_addr,&len) < 0)
        error("getpeername error");
        
    cout<<"Peer IP : "<<inet_ntoa(peer_addr.sin_addr)<<endl;
    cout<<"Peer Port : "<<peer_addr.sin_port<<endl;

    cout<<"Enter input : ";
    getline(cin,buff);

    if(send(sfd,buff.c_str(),BUFFSIZE,0) < 0)
        error("send error");

    cout<<"message sent ...\n";

}