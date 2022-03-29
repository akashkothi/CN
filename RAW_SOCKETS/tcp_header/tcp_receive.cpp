#include "../cn.h"

int main() {

    int rsfd, size;
    u_char buff[PACKETSIZE] = {'\0'};
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(struct sockaddr_in);

    if((rsfd = socket(AF_INET,SOCK_RAW,IPPROTO_TCP)) < 0)
        error("socket error");

	if(recvfrom(rsfd,buff,PACKETSIZE,0,(struct sockaddr*)&client_addr,&len) < 0)
	    error("recvfrom error");
 
    struct iphdr *ip = (struct iphdr*)buff;

    struct tcphdr *tcp = (struct tcphdr*)(buff + (ip->ihl)*4);
    
    print_ip_header(ip);

    print_tcp_header(tcp);

    cout<<"\n\tMessage : "<<(buff + (ip->ihl)*4 + (tcp->doff)*4)<<endl<<endl;

}