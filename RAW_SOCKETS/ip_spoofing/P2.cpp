#include "../cn.h"

int main() {

    int rsfd, size;
	struct iphdr *ip_header;
    char buff[BUFFSIZE] = {'\0'};
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(struct sockaddr_in);

    if((rsfd = socket(AF_INET,SOCK_RAW,8)) < 0)
        error("socket error");
        

	if(recvfrom(rsfd,buff,BUFFSIZE,0,(struct sockaddr*)&client_addr,&len) < 0)
	    error("recvfrom error");
    
    cout<<"message received ..."<<endl;
 
    ip_header = (struct iphdr*)buff;
	
    print_ip_header(ip_header);

    cout<<buff+(ip_header->ihl)*4<<endl;

    return 0;
}