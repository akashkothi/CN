#include "./cn.h"

int main() {

    key_t key;
    int msqid;

    if((key = ftok("server_msg_queue",50)) < 0)
        error("ftok error");
    
    if((msqid = msgget(key,RWX|IPC_CREAT)) < 0)
        error("msgget error");


    while(1) {

        char *ip;
        int port;

        struct mymesg mesg;

        if(msgrcv(msqid,&mesg,sizeof(mesg),0,0) < 0)
            error("msgrcv error");

        char* str = strdup(mesg.text);

        ip = strdup(strtok(str,":"));
        port = atoi(strtok(NULL,":"));

        int rsfd, opt = 1;
        struct sockaddr_in client_addr;
        socklen_t len = sizeof(struct sockaddr_in);

        if(rsfd = socket(AF_INET,SOCK_RAW,8) < 0);
            error("socket error");
        
        if(setsockopt(rsfd, IPPROTO_IP, IP_HDRINCL, &opt, sizeof(int)) < 0)
		    error("setsockopt error");
        
        char packet[BUFFSIZE] = {'\0'};

        while(1) {

            if(recvfrom(rsfd,packet,BUFFSIZE,0,(struct sockaddr*)&client_addr,&len) < 0)
                error("recvfrom error");

            struct pseudo_tcphdr *ptcp_hdr = (struct pseudo_tcphdr*)(packet + sizeof(struct iphdr));

            if(ptcp_hdr->connection_request_flag == 1)
                break;

        }

        cout<<"connection request received ..."<<endl;

        memset(packet,0,BUFFSIZE);

        struct iphdr *ip_header = (struct iphdr*)packet;
        set_ip_hdr(ip_header,8,LOCAL_HOST,inet_ntoa(client_addr.sin_addr));

        struct pseudo_tcphdr *ptcp_hdr = (struct pseudo_tcphdr*)(packet + sizeof(struct iphdr));
        set_pseudo_tcp_hdr(ptcp_hdr,8081,port,0,1,0,0);

        
        if(connect(rsfd,(struct sockaddr*)&client_addr,sizeof(struct sockaddr_in)) < 0)
            error("connect error");
        
        if(send(rsfd,packet,BUFFSIZE,0) < 0)
            error("send error");
        
        cout<<"connection accepted acknowledgement sent ..."<<endl;

        sleep(2);

        while(1) {
            
            if(recv(rsfd,packet,BUFFSIZE,0) < 0)
                error("recv error");

            ptcp_hdr = (struct pseudo_tcphdr*)(packet + sizeof(struct iphdr));
            
            if(ptcp_hdr->final_acknowledgement == 1)
                break;
        }

        cout<<"Final Acknowledgement received ..."<<endl;

        while(1);

    }

}