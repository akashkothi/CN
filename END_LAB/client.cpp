#include "./cn.h"

int main() {

    // int sfd = custom_socket(LOCAL_HOST,8080,"./client_msg_queue",111);

    key_t key;
    int msqid;

    if((key = ftok("./client_msg_queue",111)) < 0)
        error("ftok error");
    
    if((msqid = msgget(key,RWX|IPC_CREAT)) < 0)
        error("msgget error");

    struct mymesg mesg;
    
    mesg.type = 10;
    strcpy(mesg.text,"127.0.0.1:8080");

    if(msgsnd(msqid,&mesg,sizeof(mesg),0) < 0)
        error("msgsnd error");
    
    memset(mesg.text,0,BUFFSIZE);

    sleep(2);
    
    // while(1) {

        char buff[BUFFSIZE] = {'\0'};
        cin.getline(buff,BUFFSIZE);
        
        mesg.type = 1;
        strcpy(mesg.text,buff);

        if(msgsnd(msqid,&mesg,sizeof(mesg),0) < 0)
            error("msgsnd error");
        memset(mesg.text,0,BUFFSIZE);

        sleep(1);

        memset(buff,0,BUFFSIZE);
        if(msgrcv(msqid,&mesg,sizeof(mesg),1,0) < 0)
            error("msgrcv error");
        strcpy(buff,mesg.text);
        cout<<buff<<endl;

    // }

    while(1);

}