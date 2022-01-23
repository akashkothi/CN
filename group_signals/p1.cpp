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
    cout<<"Signal received ..."<<endl;
    s = 0;
}

int main() {
    key_t key;
    int msqid;
    pid_t pid[4];
    pid_t pgid;

    pid[0] = getpid();

    key = ftok("msg_queue",25);
    
    if((msqid = msgget(key,RWX|IPC_CREAT)) < 0)
        perror("msgget error");

    signal(SIGUSR2,handler2);

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

    for(int i = 1; i < 4; i++){
        if(msgrcv(msqid,&msg,sizeof(msg),18,0) < 0)
            perror("msgrcv error");
        pid[i] = atoi(msg.text);
        cout<<"Process Id received from msg queue of type 18 : "<<pid[i]<<endl;
    }

    int terminal_fd = open("/dev/tty",RWX);

    if((pgid = tcgetpgrp(terminal_fd)) < 0)
        perror("tcgetgrp error");

    msg.type = 2;
    strcpy(msg.text,to_string(pgid).c_str());

    if(msgsnd(msqid,&msg,sizeof(msg),0) < 0)
        perror("msgsnd error");

    msg.type = 3;
    strcpy(msg.text,to_string(pgid).c_str());

    if(msgsnd(msqid,&msg,sizeof(msg),0) < 0)
        perror("msgsnd error");

    msg.type = 4;
    strcpy(msg.text,to_string(pgid).c_str());

    if(msgsnd(msqid,&msg,sizeof(msg),0) < 0)
        perror("msgsnd error");

    msgctl(msqid,IPC_RMID,NULL);

    cout<<"Messages containing process group id are sent ..."<<endl;

    // sleep(10);
    // kill(0,SIGUSR2);
    

}