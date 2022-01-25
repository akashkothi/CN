#include "../cn.h"

pid_t p3, p1;
int s = 1;

struct sigaction act;

void handler(int signo, siginfo_t *info, void *context) {
    cout<<"Signal received from P3 ..."<<endl;
    p3 = info->si_pid, s = 0;
}

void handler1(int signo) {
    cout<<"Signal received from P3 ..."<<endl;
    s = 0;
}

void handler2(int signo) {
    cout<<"Signal received ..."<<endl;
    s = 0;
}

int main(){
    key_t key;
    int msqid;
    pid_t p3_pid, p1_pid;

    key = ftok("msg_queue",25);
    
    if((msqid = msgget(key,RWX|IPC_CREAT)) < 0)
        perror("msgget error");

    string pid = to_string(getpid());
    pid.push_back('\0');
    
    strcpy(msg.text,pid.c_str());

    msg.type = 4;

    signal(SIGUSR2,handler2);

    cout<<"\n----------------- In process P4"<<"("<<getpid()<<") -------------------\n\n";

    if(msgsnd(msqid,&msg,sizeof(msg),0) < 0)
        perror("msgsnd error");
    cout<<"Message containing P4 pid is sent ..."<<endl;

    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    act.sa_flags |= SA_SIGINFO; 
    act.sa_sigaction = handler;   

    if(sigaction(SIGUSR1,&act,NULL) < 0)
        perror("sigaction error");

    while(s);
    s = 1;

    cout<<"Process P3 pid recived through signal : "<<p3<<endl;
    
    p3_pid = p3;

    msg.type = 18;
    strcpy(msg.text,pid.c_str());

    if(msgsnd(msqid,&msg,sizeof(msg),0) < 0)
        perror("msgsnd error");
    
    cout<<"Message containing P4 pid is sent ..."<<endl;

    if(msgrcv(msqid,&msg,sizeof(msg),1,0) < 0)
        perror("msgrcv error");
    
    p1_pid = p1 = atoi(msg.text);
    cout<<"Process P1 pid received through msg queue : "<<p1<<endl;
    

    kill(p1_pid,SIGUSR1);
    cout<<"Signal sent to P1 ..."<<endl;


    if(msgrcv(msqid,&msg,sizeof(msg),4,0) < 0)
        perror("msgrcv error");

    int terminal_fd = open("/dev/tty",RWX);
    pid_t pgrpid = atoi(msg.text);

    if(ioctl(terminal_fd,TIOCSPGRP,&pgrpid))
        perror("ioctl error");

    cout<<"Group Id : "<<tcgetpgrp(terminal_fd)<<endl;

    cout<<"Group id received from msg queue : "<<msg.text<<endl;


}