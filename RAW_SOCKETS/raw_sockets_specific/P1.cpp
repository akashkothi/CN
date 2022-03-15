#include "../cn.h"

int main() {

    int rsfd1, rsfd2; 
    string buff = "Hello";
	struct sockaddr_in client_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    
    if((rsfd1 = socket(AF_INET,SOCK_RAW,10)) < 0)
	    error("socket error");
    
    if((rsfd2 = socket(AF_INET,SOCK_RAW,11)) < 0)
	    error("socket error");

    init_socket_address(&client_addr,LOCAL_HOST);	

    if(sendto(rsfd1,buff.c_str(),buff.size(),0,(struct sockaddr*)&client_addr,len) < 0)
        error("sendto error");

    cout<<"message sent ..."<<endl;

    buff += " I am Akash";

    if(sendto(rsfd2,buff.c_str(),buff.size(),0,(struct sockaddr*)&client_addr,len) < 0)
        error("sendto error");

    cout<<"message sent ..."<<endl;

}