#include "../cn.h"

int main() {

    int rsfd, size;
    u_char buff[PACKETSIZE] = {'\0'};
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(struct sockaddr_in);

    if((rsfd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP)) < 0)
        error("socket error");

    while(1) {

	    if(recvfrom(rsfd,buff,PACKETSIZE,0,(struct sockaddr*)&client_addr,&len) < 0)
	        error("recvfrom error");
    
        struct iphdr *ip = (struct iphdr*)buff;

        struct tcphdr *tcp = (struct tcphdr*)(buff + (ip->ihl)*4);

        cout<<endl<<"Source Port : "<<ntohs(tcp->source)<<endl;
        cout<<"Destination Port : "<<ntohs(tcp->dest)<<endl;
    }

}