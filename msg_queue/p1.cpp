#include "../cn.h"

int main() {

    key_t key;
    int msqid;
    int pid[3];

    char *messages[] = {"Message from P1 to P2","Message from P1 to P3","Message from P1 to P4"};

    key = ftok("msg_queue",50);
    
    if((msqid = msgget(key,RWX|IPC_CREAT)) < 0)
        perror("msgget error");

    cout<<"\n----------------- In process P1 -------------------\n\n";

    for(int i = 0; i < 3; i++){
        if(msgrcv(msqid,&msg,sizeof(msg),i+2,0) < 0)
            perror("msgrcv error");
        pid[i] = atoi(msg.text);
    }

    for(int i = 0; i < 3; i++) {
        msg.type = pid[i];
        strcpy(msg.text,messages[i]);
        if(msgsnd(msqid,&msg,sizeof(msg),0) < 0)
            perror("msgsnd error");
        cout<<"Message sent to process with pid "<<pid[i]<<" ..."<<endl;    
    }

    msgctl(msqid,IPC_RMID,NULL);
    
}