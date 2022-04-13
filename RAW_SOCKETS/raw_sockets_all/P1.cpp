#include "../cn.h"

int main() {

    int rsfd, nrsfd; 
    string buff = "Hello";
	struct sockaddr_in client_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    
    if((rsfd = socket(AF_INET,SOCK_RAW,8)) < 0)
	    error("socket error");
        
    init_socket_address(&client_addr,LOCAL_HOST);	

    if(connect(rsfd,(struct sockaddr*)&client_addr,len) < 0)
        error("connect error");

    dup2(rsfd,1);
    cout<<buff<<endl;
}