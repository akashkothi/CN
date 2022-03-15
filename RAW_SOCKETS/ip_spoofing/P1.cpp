#include "../cn.h"

int main() {

    int rsfd, opt = 1; 
    struct iphdr *ip_header;
    char buff[BUFFSIZE] = {'\0'};
	struct sockaddr_in client_addr;
    socklen_t len = sizeof(struct sockaddr_in);

    ip_header = (struct iphdr*)buff;

    ip_header->version = 4;
    ip_header->ihl = 5;
	ip_header->tos = 0; 
	ip_header->tot_len = BUFFSIZE;
	ip_header->id = htons(218);
	ip_header->ttl = 100; 
	ip_header->protocol = 8; 
	ip_header->saddr = inet_addr("192.168.32.12");
	ip_header->daddr = inet_addr(LOCAL_HOST);

    strcpy(buff+20,"Hello");

    if((rsfd = socket(AF_INET,SOCK_RAW,8)) < 0)
	    error("socket error");
    
    if(setsockopt(rsfd, IPPROTO_IP, IP_HDRINCL, &opt, sizeof(int)) < 0)
		error("setsockopt error");

    init_socket_address(&client_addr,LOCAL_HOST);	

    if(sendto(rsfd,buff,BUFFSIZE,0,(struct sockaddr*)&client_addr,len) < 0)
        error("sendto error");

    cout<<"message sent ..."<<endl;

}