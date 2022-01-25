#include "../cn.h"

int main() {

    key_t key;
    int msqid;

    key = ftok("msg_queue",50);
    
    if((msqid = msgget(key,RWX|IPC_CREAT)) < 0)
        perror("msgget error");

    string pid = to_string(getpid());
    pid.push_back('\0');
    
    msg.type = 4;

    for(int i = 0; i < pid.size(); i++)
        msg.text[i] = pid[i];

    cout<<"\n----------------- In process P4"<<"("<<getpid()<<") -------------------\n\n";

    if(msgsnd(msqid,&msg,sizeof(msg),0) < 0)
        perror("msgsnd error");

    cout<<"Message sent ..."<<endl;

    if(msgrcv(msqid,&msg,sizeof(msg),getpid(),0) < 0)
        perror("msgrcv error");
    
    cout<<msg.text<<endl;

}