#include "./cn.h"

string A1[3] = {"t1","t3","t7"};
string A2[2] = {"t2","t5"};

const char* locations[4] = {"l1","l2","l3","l4"};

const char* info_places = "Available Tourist Places : t1 t2 t3 t4";
const char* info_location = "Pickup locations : l1 l2 l3 l4";

// void* broadcast(void* arg) {
//     int rsfd = *(int *)arg;

//     while(1) {




//     }
// }


int get_agent(string request) {
    
    for(string taxi : A1) {
        if(taxi == request)
            return 1;
    }
    
    for(string taxi : A2) {
        if(taxi == request)
            return 2;
    }
    
    return -1;
}

int get_location(char* location) {
    for(int i = 0; i < 4; i++){
        if(strcmp(locations[i],location) == 0)
            return i;
    }
    return -1;
}

int main() {

    pollfd psfd[100];
    int sfd, nsfd, rsfd, count = 1;
    struct sockaddr_in server_addr,tourist_addr, my_addr;
    socklen_t len = sizeof(struct sockaddr_in);

    int msqid;
    key_t msqkey;

    int usfd[4];
    socklen_t taxi_len = sizeof(struct sockaddr_un);
    struct sockaddr_un location_addr[4], taxi_addr;
    


    if((msqkey = ftok("./msg_queue",89)) < 0)
        error("ftok error");
    
    if((msqid = msgget(msqkey,RWX|IPC_CREAT)) < 0)
        error("msgget error");



    
    init_socket_address(&server_addr,LOCAL_HOST,PORT);

    if((sfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
        error("socket error");
    
    if(bind(sfd,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0)
        error("bind error");

    if(listen(sfd,BACKLOG) < 0) 
        error("listen error");
    
    psfd[0].fd = sfd;
    psfd[0].events = POLLIN;
    


    if((rsfd = socket(AF_INET,SOCK_RAW,8)) < 0)
	    error("socket error");
    
    init_socket_address(&my_addr,LOCAL_HOST);



    for(int i = 0; i < 4; i++) {
        
        unlink(locations[i]);

        usfd[i] = uds_socket(SOCK_STREAM,locations[i],location_addr[i]);
    
        if(bind(usfd[i],(struct sockaddr*)&location_addr[i],len) < 0)
            error("bind error");

    }

    while(1) {
        
        if(poll(psfd,count,500) < 0)
            error("poll error");
        else {
            for(int i = 0; i < count; i++) {
                if(psfd[i].revents == POLLIN) {
                    if(i == 0) {

                        if((nsfd = accept(psfd[i].fd,(struct sockaddr*)&tourist_addr,&len)) < 0)
                            error("accept error");
                        
                        psfd[count].fd = nsfd;
                        psfd[count++].events = POLLIN;

                        if(send(nsfd,"Can you please provide the details of location and type of taxi service needed ?",BUFFSIZE,0) < 0)
                            error("send error");
                    
                    }
                    else {

                        int agent, idx, nusfd;

                        char service[BUFFSIZE] = {'\0'};
                        char location[BUFFSIZE] = {'\0'};

                        if(recv(psfd[i].fd,service,2,0) < 0)
                            error("recv error");
                        cout<<"Tourist place : "<<service<<endl;
                        
                        if(recv(psfd[i].fd,location,2,0) < 0)
                            error("recv error");
                        cout<<"Pickup location : "<<location<<endl;
                        
                        string service_(service);

                        if((agent = get_agent(service)) < 0)
                            error("get_agent error");

                        cout<<"Agent number : "<<agent<<endl;

                        msg.type = agent;
                        strcpy(msg.text,location);

                        if(msgsnd(msqid,&msg,sizeof(msg),0) < 0)
                            error("msgsnd error");
                        
                        cout<<"Location details sent to Agent through message queue..."<<endl;

                        if((idx = get_location(location)) < 0)
                            error("get_location error");
                        
                        cout<<"Location idx : "<<idx<<endl;
                        
                        if((nusfd = accept(usfd[idx],(struct sockaddr*)&taxi_addr,&taxi_len)) < 0)
                            error("UDS accept error");
                        
                        if(send_fd(nusfd,psfd[i].fd) < 0)
                            error("send_fd error");

                        cout<<"nsfd sent to taxi ..."<<endl;

                        close(psfd[i].fd);

                    }
                }
            }
        }

        if(sendto(rsfd,info_places,BUFFSIZE,0,(struct sockaddr*)&my_addr,len) < 0)
            error("sendto error");
        if(sendto(rsfd,info_location,BUFFSIZE,0,(struct sockaddr*)&my_addr,len) < 0)
            error("sendto error");
    }

}