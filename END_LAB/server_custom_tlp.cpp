#include "./cn.h"

// struct rsfd_msqid {
//     int rsfd, msqid;
// };

// void* receiver(void* arg) {
    
//     struct rsfd_msqid *fds = (struct rsfd_msqid*)arg;

//     int rsfd = fds->rsfd;
//     int msqid = fds->msqid;

//     while(1) {

//         char packet[BUFFSIZE] = {'\0'};

//         if(recv(rsfd,packet,BUFFSIZE,0) < 0)
//             error("recv error");

//         struct mymesg mesg;
//         mesg.type = 10;
//         strcpy(mesg.text,packet + sizeof(struct iphdr) + sizeof(struct pseudo_tcphdr));

//         if(msgsnd(msqid,&mesg,sizeof(mesg),0) < 0)
//             error("msgsnd error");
//     }

// }


int main() {

    key_t key;
    int msqid;

    if((key = ftok("./server_msg_queue",110)) < 0)
        error("ftok error");
    
    if((msqid = msgget(key,RWX|IPC_CREAT)) < 0)
        error("msgget error");

    while(1) {

        char *ip;
        int port, dstport;

        struct mymesg mesg;

        if(msgrcv(msqid,&mesg,sizeof(mesg),0,0) < 0)
            error("msgrcv error");
        

        char* str = strdup(mesg.text);

        ip = strdup(strtok(str,":"));
        port = atoi(strtok(NULL,":"));

    
        int rsfd, opt = 1;
        struct sockaddr_in client_addr;
        socklen_t len = sizeof(struct sockaddr_in);

         
        if((rsfd = socket(AF_INET,SOCK_RAW,8)) < 0)
            error("socket error");
        
        if(setsockopt(rsfd, IPPROTO_IP, IP_HDRINCL, &opt, sizeof(int)) < 0)
		    error("setsockopt error");
        
        char packet[BUFFSIZE] = {'\0'};

        while(1) {

            if(recvfrom(rsfd,packet,BUFFSIZE,0,(struct sockaddr*)&client_addr,&len) < 0)
                error("recvfrom error");

            struct pseudo_tcphdr *ptcp_hdr = (struct pseudo_tcphdr*)(packet + sizeof(struct iphdr));

            if(ptcp_hdr->connection_request_flag == 1){
                dstport = ptcp_hdr->destination_port;
                break;
            }

        }

        cout<<"connection request received ..."<<endl;

        memset(packet,0,BUFFSIZE);

        struct iphdr *ip_header = (struct iphdr*)packet;
        set_ip_hdr(ip_header,8,LOCAL_HOST,inet_ntoa(client_addr.sin_addr));

        struct pseudo_tcphdr *ptcp_hdr = (struct pseudo_tcphdr*)(packet + sizeof(struct iphdr));
        set_pseudo_tcp_hdr(ptcp_hdr,port,8081,0,1,0,0,0,0);

        
        // if(connect(rsfd,(struct sockaddr*)&client_addr,sizeof(struct sockaddr_in)) < 0)
        //     error("connect error");
        
        if(sendto(rsfd,packet,BUFFSIZE,0,(struct sockaddr*)&client_addr,sizeof(struct sockaddr_in)) < 0)
            error("sendto error");
        
        cout<<"connection accepted acknowledgement sent ..."<<endl;


        while(1) {
            

            if(recvfrom(rsfd,packet,BUFFSIZE,0,(struct sockaddr*)&client_addr,&len) < 0)
                error("recvfrom error");

            // if(recv(rsfd,packet,BUFFSIZE,0) < 0)
            //     error("recv error");

            ptcp_hdr = (struct pseudo_tcphdr*)(packet + sizeof(struct iphdr));
            
            if(ptcp_hdr->final_acknowledgement == 1)
                break;   

        }

        cout<<"Final Acknowledgement received ..."<<endl;

        // struct rsfd_msqid fds;

        // fds.rsfd = rsfd;
        // fds.msqid = msqid;

        // pthread_t thread;

        // pthread_create(&thread,NULL,receiver,&fds);

        while(1) {
            
            cout<<"Waiting for message ..."<<endl;

            // if(recv(rsfd,packet,BUFFSIZE,0) < 0)
            //     error("recv error");

            if(recvfrom(rsfd,packet,BUFFSIZE,0,(struct sockaddr*)&client_addr,&len) < 0)
                error("recvfrom error");
            
            
            struct mymesg mesg;

            mesg.type = 1;
            strcpy(mesg.text,(packet + sizeof(struct iphdr) + sizeof(struct pseudo_tcphdr)));

            cout<<mesg.text<<endl;

            if(msgsnd(msqid,&mesg,sizeof(mesg),0) < 0)
                error("msgsnd error");
            
            int datalen = strlen(mesg.text);
            
            cout<<"message sent to server"<<endl;
            
            memset(msg.text,0,BUFFSIZE);

            sleep(1);

            if(msgrcv(msqid,&mesg,sizeof(mesg),1,0) < 0)
                error("msgrcv error");
            
            memset(packet,0,BUFFSIZE);

            ip_header = (struct iphdr*)packet;
            set_ip_hdr(ip_header,8,LOCAL_HOST,inet_ntoa(client_addr.sin_addr));

            ptcp_hdr = (struct pseudo_tcphdr*)(packet + sizeof(struct iphdr));
            set_pseudo_tcp_hdr(ptcp_hdr,port,dstport,0,0,0,1,datalen,0);
            
            strcpy((packet + sizeof(struct iphdr) + sizeof(struct pseudo_tcphdr)),mesg.text);

            // if(send(rsfd,packet,BUFFSIZE,0) < 0)
            //     error("send errro");

            if(sendto(rsfd,packet,BUFFSIZE,0,(struct sockaddr*)&client_addr,sizeof(struct sockaddr_in)) < 0)
                error("sendto error");
            
            sleep(2);
        }

    }

}