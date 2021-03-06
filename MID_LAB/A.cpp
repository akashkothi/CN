#include "./cn.h"

void handler(int signo) {
    cout<<"Taxi breakdown occured(Agent)"<<endl;
}

int main(int argc, char* argv[]) {

    if(argc != 2){
        cout<<"Agent number should be given as command line argument"<<endl;
        exit(EXIT_FAILURE);
    }

    signal(SIGUSR1,handler);

    string pid = to_string(getpid());

    key_t msqkey;
    int msqid, agent_no = atoi(argv[1]);

    if((msqkey = ftok("./msg_queue",89)) < 0)
        error("ftok error");
    
    if((msqid = msgget(msqkey,RWX|IPC_CREAT)) < 0)
        error("msgget error");

    while(1) {

        if(msgrcv(msqid,&msg,sizeof(msg),agent_no,0) < 0){
            if(errno == EINTR)
                continue;
            error("msgrcv error");
        }

        cout<<"Location details received through message queue ..."<<endl;

        cout<<"Pick up location : "<<msg.text<<endl;
                
        if(fork() == 0) {
            execl("./X.exe","./X.exe",msg.text,pid.c_str(),NULL);
            cout<<"Location details given to taxi ..."<<endl;
        }

    }


}