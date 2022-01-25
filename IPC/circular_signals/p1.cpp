#include "../cn.h"

pid_t p4_pid, p2_pid;
int s = 1;

struct sigaction act;

void handler(int signo, siginfo_t *info, void *context) {
    cout<<"Signal received from P4 ..."<<endl;
    p4_pid = info->si_pid, s = 0;
}

void handler1(int signo) {
    cout<<"Signal received from P4 ..."<<endl;
    s = 0;
}

void handler2(int signo) {
    cout<<"Signal received from P2 ..."<<endl;
    s = 0;
}

int main() {
    key_t key;
    int msqid;

    key = ftok("msg_queue",25);
    
    if((msqid = msgget(key,RWX|IPC_CREAT)) < 0)
        perror("msgget error");

    cout<<"\n----------------- In process P1"<<"("<<getpid()<<") -------------------\n\n";

    if(msgrcv(msqid,&msg,sizeof(msg),2,0) < 0)
        perror("msgrcv error");
    
    p2_pid = atoi(msg.text);
    cout<<"Process P2 pid received through msg queue : "<<p2_pid<<endl;

    kill(p2_pid,SIGUSR1);
    cout<<"Signal sent to P2 ..."<<endl;

    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    act.sa_flags |= SA_SIGINFO; 
    act.sa_sigaction = handler;   

    if(sigaction(SIGUSR1,&act,NULL) < 0)
        perror("sigaction error");

    while(s);
    s = 1;
    cout<<"Process P4 pid received through signal : "<<p4_pid<<endl;

    msgctl(msqid,IPC_RMID,NULL);

    cout<<"\n------------- Circular Signalling -----------\n\n";

    signal(SIGUSR1,handler1);
    for(int i = 0; i < 3; i++) {
        kill(p2_pid,SIGUSR1);
        cout<<"Signal sent to P2 ..."<<endl;
        while(s);
        s = 1;
    }

    cout<<"\n------------- Reverese Circular Signalling -----------\n\n";

    signal(SIGUSR2,handler2);
    for(int i = 0; i < 3; i++) {
        kill(p4_pid,SIGUSR2);
        cout<<"Signal sent to P4 ..."<<endl;
        while(s);
        s = 1;
    }

}