#include "../cn.h"

int main() {

    int rsfd; 
    string buff = "Hello";
	struct sockaddr_in client_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    
    if((rsfd = socket(AF_INET,SOCK_RAW,IPPROTO_TCP)) < 0)
	    error("socket error");
        
    init_socket_address(&client_addr,LOCAL_HOST);	

    if(sendto(rsfd,buff.c_str(),buff.size(),0,(struct sockaddr*)&client_addr,len) < 0)
        error("sendto error");

    cout<<"message sent ..."<<endl;

}