#include "../cn.h"

int main() {

    int udp_sfd, port = 9090;
    struct sockaddr_in my_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    unordered_map <int,int> ssfdj_nsfdj, nsfdj_ssfdj;
    unordered_map <int,struct sockaddr_in> sfdi_Si_addr; 

    init_socket_address(&my_addr,LOCAL_HOST,PORT);

    if((udp_sfd = socket(AF_INET,SOCK_DGRAM,0)) < 0)
        error("socket error");

    if(bind(udp_sfd,(struct sockaddr*)&my_addr,len) < 0)
        error("bind error");
    
    pollfd pudp_sfd[1];
    pudp_sfd[0].fd = udp_sfd;
    pudp_sfd[0].events = POLLIN;

    pollfd sfd[100];
    int c1 = 0;

    pollfd nsfd[100];
    int c2 = 0;

    pollfd ssfd[100];
    int c3 = 0;

    while(1) {

        if(poll(pudp_sfd,1,500) < 0)
            error("poll error");
        
        if(pudp_sfd[0].revents == POLLIN) {
            
            int sfdi;
            socklen_t len = sizeof(struct sockaddr_in);
            struct sockaddr_in Si_addr, client_addr, sfdi_addr;

            if(recvfrom(udp_sfd,&Si_addr,sizeof(Si_addr),0,(struct sockaddr*)&client_addr,&len) < 0)
                error("recvfrom error");
            
            cout<<"connection oriented port address received ..."<<endl;

            init_socket_address(&sfdi_addr,LOCAL_HOST,port++);

            if((sfdi = socket(AF_INET,SOCK_STREAM,0)) < 0)
                error("socket error");
            
            if(bind(sfdi,(struct sockaddr*)&sfdi_addr,len) < 0)
                error("bind error");
            
            if(listen(sfdi,BACKLOG) < 0)
                error("listen error");
            
            sfdi_Si_addr[sfdi] = Si_addr; // mapping sfdi -> Si_addr

            sfd[c1].events = POLLIN;
            sfd[c1++].fd = sfdi;
        }

        if(poll(sfd,c1,500) < 0)
            error("poll error");
        
        for(int i = 0; i < c1; i++) {
            if(sfd[i].revents == POLLIN) {

                int nsfdj, ssfdj;
                struct sockaddr_in client_addr;
                socklen_t len = sizeof(struct sockaddr_in);

                if((nsfdj = accept(sfd[i].fd,(struct sockaddr*)&client_addr,&len)) < 0)
                    error("accept error");
                
                nsfd[c2].events = POLLIN;
                nsfd[c2++].fd = nsfdj;
                
                if((ssfdj = socket(AF_INET, SOCK_STREAM,0)) < 0)
                    error("socket error");
                
                if(connect(ssfdj,(struct sockaddr*)&sfdi_Si_addr[sfd[i].fd],len) < 0)
                    error("connect error");

                ssfd[c3].events = POLLIN;
                ssfd[c3++].fd = ssfdj;

                ssfdj_nsfdj[ssfdj] = nsfdj; // mapping ssfdj -> nsfdj
                nsfdj_ssfdj[nsfdj] = ssfdj; // mapping nsfdj -> ssfdj

            }

            if(poll(nsfd,c2,500) < 0)
                error("poll error");
            
            for(int i = 0; i < c2; i++){
                if(nsfd[i].revents == POLLIN) {
                    
                    char buff[BUFFSIZE] = {'\0'};
                    
                    if(recv(nsfd[i].fd,buff,BUFFSIZE,0) < 0)
                        error("recv error");

                    if(send(nsfdj_ssfdj[nsfd[i].fd],buff,BUFFSIZE,0) < 0)
                        error("send error");
                }
            }

            if(poll(ssfd,c3,500) < 0)
                error("poll error");
            
            for(int i = 0; i < c3; i++) {
                if(ssfd[i].revents == POLLIN) {
                    
                    char buff[BUFFSIZE] = {'\0'};

                    if(recv(ssfd[i].fd,buff,BUFFSIZE,0) < 0)
                        error("recv error");

                    if(send(ssfdj_nsfdj[ssfd[i].fd],buff,BUFFSIZE,0) < 0)
                        error("send error");

                }
            }

        }

    } 

}