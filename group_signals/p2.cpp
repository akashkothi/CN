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
    cout<<"Signal received  ..."<<endl;
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

    signal(SIGUSR1,handler2);

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

    msg.type = 18;
    strcpy(msg.text,pid.c_str());

    if(msgsnd(msqid,&msg,sizeof(msg),0) < 0)
        perror("msgsnd error");
    
    cout<<"Message containing P2 pid is sent ..."<<endl;

    kill(p3,SIGUSR1);
    cout<<"Signal sent to P3 ..."<<endl;

    if(msgrcv(msqid,&msg,sizeof(msg),2,0) < 0)
        perror("msgrcv error");

    int terminal_fd = open("/dev/tty",RWX);
    pid_t pgrpid = atoi(msg.text);

    if(ioctl(terminal_fd,TIOCSPGRP,&pgrpid))
        perror("ioctl error");

    cout<<"Group Id : "<<tcgetpgrp(terminal_fd)<<endl;

    cout<<"Group id received from msg queue : "<<msg.text<<endl;

    
}