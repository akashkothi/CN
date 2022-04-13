#include "../cn.h"

int main() {

    int rsfd, nsfd, size;
	struct iphdr *ip;
    char buff[BUFFSIZE] = {'\0'};
    struct sockaddr_in client_addr, server_addr;
    socklen_t len = sizeof(struct sockaddr_in);

    if((rsfd = socket(AF_INET,SOCK_RAW,8)) < 0)
        error("socket error");
    
    init_socket_address(&server_addr,LOCAL_HOST);
    
    if(connect(rsfd,(struct sockaddr*)&server_addr,len) < 0)
        error("connect error");
    
    dup2(rsfd,0);

    cin.getline(buff,BUFFSIZE);
 
    ip = (struct iphdr*)buff;
    
	cout<<buff+(ip->ihl)*4<<endl;
}