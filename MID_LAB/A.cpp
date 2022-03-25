#include "./cn.h"

int main(int argc, char* argv[]) {


    key_t msqkey;
    int msqid, agent_no = atoi(argv[1]);

    if((msqkey = ftok("msg_queue",50)) < 0)
        error("ftok error");
    
    if((msqid = msgget(msqkey,RWX|IPC_CREAT)) < 0)
        perror("msgget error");

    while(1) {

        if(msgrcv(msqid,&msg,sizeof(msg),agent_no,0) < 0)
            error("msgrcv error");
        
        cout<<"Location details received through message queue ..."<<endl;
        
        if(fork() == 0) {
            execl("./X.exe","./X.exe",msg.text,NULL);
            cout<<"Location details given to taxi ..."<<endl;
        }

    }


}