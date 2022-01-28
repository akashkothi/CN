#include "../cn.h"

int main() {

    pid_t pid;
    int sfd, nsfd;
    struct sockaddr_in server_addr,client_addr;
    socklen_t len = sizeof(client_addr);

    init_socket_address(&server_addr,LOCAL_HOST,PORT);    

    if((sfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
        perror("socket error");
    
    if(bind(sfd,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0)
        perror("bind error");

    if(listen(sfd,BACKLOG) < 0) 
        perror("listen error");

    cout<<"waiting for connection ...\n";

    if((nsfd = accept(sfd,(struct sockaddr *)&client_addr,&len)) < 0)
        perror("accept error");
    
    cout<<"connection established ...\n";

    int fd = open("./program.cpp",O_WRONLY|O_CREAT|O_TRUNC); // create a cpp file store the code read from client
    char buff[BUFFSIZE] = {'\0'};

    while(recv(nsfd,buff,BUFFSIZE,0))
        write(fd,buff,BUFFSIZE);

    cout<<"file received ..."<<endl;

    system("g++ -w program.cpp -o P.exe");

    cout<<"file compiled ..."<<endl;

    if((pid = fork()) < 0)
        perror("fork error");
    else if(pid > 0) {

        wait(NULL); 

        cout<<"file executed successfully ..."<<endl;

        int otfd = open("./OT.txt",R);
        int poutfd = open("./Pout.txt",R);
        bool issame = true;

        char buff1[2] = {'\0'};
        char buff2[2] = {'\0'};

        while(issame && read(otfd,buff1,1)&&read(poutfd,buff2,1)) {
            if(*buff1 != *buff2)
                issame = false;
        }

        if(read(otfd,buff1,1)||read(poutfd,buff2,1))
            issame = false;

        cout<<"output comparison completed ..."<<endl;
        
        if(issame) {
            if(send(nsfd,"Test case passed",BUFFSIZE,0) < 0)
                error("send error");
        }
        else {
            if(send(nsfd,"Test case failed",BUFFSIZE,0) < 0)
                error("send error");
        }

    }
    else {
        close(nsfd);
        close(sfd);
        int itfd = open("./IT.txt",R);
        int poutfd = open("./Pout.txt",O_WRONLY|O_TRUNC); // open Pout.txt which is created manually 

        dup2(itfd,STDIN_FILENO);
        dup2(poutfd,STDOUT_FILENO);

        execl("./P.exe","./P.exe",NULL);

    }



    
}