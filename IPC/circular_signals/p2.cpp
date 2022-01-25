#include "../cn.h"

pid_t p1, p3;
int s = 1;

struct sigaction act;

void handler(int signo, siginfo_t *info, void *context) {
    cout<<"Signal received from P1 ..."<<endl;
    p1 = info->si_pid, s = 0;
}

void handler1(int signo) {
    cout<<"Signal received from P1 ..."<<endl;
    s = 0;
}

void handler2(int signo) {
    cout<<"Signal received from P3 ..."<<endl;
    s = 0;
}

int main() {
    key_t key;
    int msqid;
    pid_t p1_pid, p3_pid;

    key = ftok("msg_queue",25);
    
    if((msqid = msgget(key,RWX|IPC_CREAT)) < 0)
        perror("msgget error");

    string pid = to_string(getpid());
    pid.push_back('\0');
    
    strcpy(msg.text,pid.c_str());

    msg.type = 2;

    cout<<"\n----------------- In process P2"<<"("<<getpid()<<") -------------------\n\n";

    if(msgsnd(msqid,&msg,sizeof(msg),0) < 0)
        perror("msgsnd error");

    cout<<"Message containing P2 pid is sent ..."<<endl;

    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    act.sa_flags |= SA_SIGINFO; 
    act.sa_sigaction = handler;   

    if(sigaction(SIGUSR1,&act,NULL) < 0)
        perror("sigaction error");

    while(s);
    s = 1;

    cout<<"Process P1 pid received through signal : "<<p1<<endl;
    
    p1_pid = p1;

    if(msgrcv(msqid,&msg,sizeof(msg),3,0) < 0)
        perror("msgrcv error");

    p3 = atoi(msg.text);
    cout<<"Process P3 pid received through msg queue : "<<p3<<endl;

    p3_pid = p3;

    msg.type = 1;
    strcpy(msg.text,to_string(p1_pid).c_str());

    if(msgsnd(msqid,&msg,sizeof(msg),0) < 0)
        perror("msgsnd error");
    
    cout<<"Message containing P1 pid is sent ..."<<endl;

    kill(p3,SIGUSR1);
    cout<<"Signal sent to P3 ..."<<endl;

    cout<<"\n------------- Circular Signalling -----------\n\n";

    signal(SIGUSR1,handler1);
    for(int i = 0; i < 3; i++) {
        while(s);
        s = 1;
        kill(p3_pid,SIGUSR1);
        cout<<"Signal sent to P3 ..."<<endl;
    }

    cout<<"\n------------- Reverse Circular Signalling -----------\n\n";

    signal(SIGUSR2,handler2);
    for(int i = 0; i < 3; i++) {
        while(s);
        s = 1;
        kill(p1_pid,SIGUSR2);
        cout<<"Signal sent to P1 ..."<<endl;       
    }
    
}