#include "../cn.h"

int main() {

    int sfd;
    struct sockaddr_in server_addr;

    init_socket_address(&server_addr,LOCAL_HOST,8080);

    cout<<"\n   --------------ARTIST---------------\n"<<endl;
    
    if((sfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
        error("socket error");

    if(connect(sfd,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0)
        error("connect error");
    
    cout<<"connected to the screen ..."<<endl;

    string performance;

    do{
        
        performance.clear();
        getline(cin,performance);

    }while(send(sfd,performance.c_str(),performance.size(),0) > 1);

    char ques[1024] = {'\0'};
    string answer;

    if(recv(sfd,ques,1024,0) < 0)
        error("recv error");
    
    cout<<"Question from Judge : "<<ques<<endl;
    cout<<"Answer : ";
    getline(cin,answer);

    if(send(sfd,answer.c_str(),answer.size(),0) < 0)
        error("send error");

}