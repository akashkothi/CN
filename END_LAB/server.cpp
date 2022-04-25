#include "./cn.h"

int main() {

    // int sfd = custom_socket(LOCAL_HOST,8080,"./server_msg_queue",110);

    key_t key;
    int msqid;

    if((key = ftok("./server_msg_queue",110)) < 0)
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
        if(msgrcv(msqid,&mesg,sizeof(mesg),1,0) < 0)
            error("msgrcv error");
        strcpy(buff,mesg.text);
        cout<<buff<<endl;
        
        sleep(2);

        char buff2[BUFFSIZE] = {'\0'};
        cin.getline(buff2,BUFFSIZE);
        mesg.type = 1;
        strcpy(mesg.text,buff2);
        if(msgsnd(msqid,&mesg,sizeof(mesg),0) < 0)
            error("msgsnd error");

    // }

    while(1);

}