#include "../cn.h"
#define NO_OF_PROCESSES 4

void helper(int id = 1) {
    
    if(id == NO_OF_PROCESSES){
        cout<<"Process P"<<id<<" with id "<<getpid()<<" started executing"<<endl;
        cout<<"Process P"<<id<<" created by parent process "<<getppid()<<endl;
        cout<<"Process P"<<id<<"("<<getpid()<<") completed executing"<<endl;        
    }
    else {
        pid_t pid;

        cout<<"Process P"<<id<<" with id "<<getpid()<<" started executing"<<endl;
        cout<<"Process P"<<id<<" created by parent process "<<getppid()<<endl;

        if((pid = fork()) < 0)
            cout<<"fork error"<<endl;
        else if(pid == 0)
            helper(id+1);
        else {
            wait(NULL);
            cout<<"Process P"<<id<<"("<<getpid()<<") completed executing"<<endl;
        } 
    }
    return;
}

int main() {
    helper();
    return 0;
}
