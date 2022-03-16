#include "../cn.h"

int main() {

    int rsfd, sfd;
    struct iphdr *ip_header;
    char buff[BUFFSIZE] = {'\0'};
    socklen_t len = sizeof(struct sockaddr_in);
    struct sockaddr_in raw_server_addr, tcp_server_addr;

    if((rsfd = socket(AF_INET,SOCK_RAW,8)) < 0)
        error("socket error");

    init_socket_address(&tcp_server_addr,LOCAL_HOST,PORT);

    if((sfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
        error("socket error");


	if(recvfrom(rsfd,buff,BUFFSIZE,0,(struct sockaddr*)&raw_server_addr,&len) < 0)
	    error("recvfrom error");
    
    cout<<"\n\t-------------- Required Documents -------------\n"<<endl;

    ip_header = (struct iphdr*)buff;
	cout<<"\t\t\t"<<buff+(ip_header->ihl)*4<<endl;

    if(recvfrom(rsfd,buff,BUFFSIZE,0,(struct sockaddr*)&raw_server_addr,&len) < 0)
	    error("recvfrom error");

    ip_header = (struct iphdr*)buff;
	cout<<"\t\t\t"<<buff+(ip_header->ihl)*4<<endl;


    if(recvfrom(rsfd,buff,BUFFSIZE,0,(struct sockaddr*)&raw_server_addr,&len) < 0)
	    error("recvfrom error");
    
    ip_header = (struct iphdr*)buff;
	cout<<"\t\t\t"<<buff+(ip_header->ihl)*4<<endl;


    if(connect(sfd,(struct sockaddr *)&tcp_server_addr,sizeof(tcp_server_addr)) < 0)
        error("connect error");


    for(int i = 0; i < 3; i++) {

        char opinion[BUFFSIZE] = {'\0'};
        char document[BUFFSIZE] = {'\0'};

        if(send(sfd,document,BUFFSIZE,0) < 0)
            error("send error");
        
        cout<<"Document sent ..."<<endl;

        if(recv(sfd,opinion,BUFFSIZE,0) < 0)
            error("recv error");
        
        cout<<opinion<<endl;

    }


} 