#include "./cn.h"

int main() {

    int sfd, rsfd;
    struct iphdr *ip;

    char buff[BUFFSIZE] = {'\0'};

    struct sockaddr_in server_addr, my_addr;
    socklen_t len = sizeof(struct sockaddr_in);

    init_socket_address(&server_addr,LOCAL_HOST,PORT);
    
    if((sfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
        error("socket error");

    init_socket_address(&my_addr,LOCAL_HOST);
    
    if((rsfd = socket(AF_INET,SOCK_RAW,8)) < 0)
	    error("socket error");

    if(connect(sfd,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0)
        error("connect error");
    
    cout<<"Connected to tourism server ..."<<endl;

    if(recvfrom(rsfd,buff,BUFFSIZE,0,(struct sockaddr*)&my_addr,&len) < 0)
	    error("recvfrom error");
 
    ip = (struct iphdr*)buff;
    
	cout<<endl<<buff+(ip->ihl)*4<<endl;

    memset(buff,0,BUFFSIZE);

    if(recvfrom(rsfd,buff,BUFFSIZE,0,(struct sockaddr*)&my_addr,&len) < 0)
	    error("recvfrom error");
 
    ip = (struct iphdr*)buff;
    
	cout<<buff+(ip->ihl)*4<<endl<<endl;

    memset(buff,0,BUFFSIZE);

    if(recv(sfd,buff,BUFFSIZE,0) < 0)
        cout<<"recv error"<<endl;
    
    cout<<buff<<endl;

    string place, location;

    cout<<"Enter the Tourist place : ";
    cin>>place;
    place.push_back('\0');
    
    if(send(sfd,place.c_str(),place.size(),0) < 0)
        error("send error");
    
    cout<<"Enter the pick up location : ";
    cin>>location;
    location.push_back('\0');
    
    if(send(sfd,location.c_str(),location.size(),0) < 0)
        error("send error");

    while(1) {

        string input;
        getline(cin,input); 

        if(send(sfd,input.c_str(),BUFFSIZE,0) < 0)
            error("send error");

    }

}