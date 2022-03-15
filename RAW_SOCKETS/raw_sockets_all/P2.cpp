#include "../cn.h"

int main() {

    int rsfd, size;
	struct iphdr *ip;
    char buff[BUFFSIZE] = {'\0'};
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(struct sockaddr_in);

    if((rsfd = socket(AF_INET,SOCK_RAW,10)) < 0)
        error("socket error");

	if(recvfrom(rsfd,buff,BUFFSIZE,0,(struct sockaddr*)&client_addr,&len) < 0)
	    error("recvfrom error");
    
    cout<<"message received ..."<<endl;
 
    ip = (struct iphdr*)buff;
	cout<<buff+(ip->ihl)*4<<endl;

}