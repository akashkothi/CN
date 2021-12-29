#include "../cn.h"

int main() {

    pid_t p2, p3;
    cout<<"Process id : "<<getpid()<<endl;
    (p2 = fork()) && (p3 = fork());
    if(p2 == 0) {
        cout<<"P2 started executing"<<endl;
        for(int i = 0; i < 2; i++)
            sleep(1);
    }
    else if(p3 == 0) { 
        cout<<"P3 started executing"<<endl;
        for(int i = 0; i < 3; i++)
            sleep(1);
    } 
    else {
       pid_t pid = wait(NULL);
       
       if(pid == p2)
        cout<<"P2 terminated"<<endl;
       else if(pid == p3)
        cout<<"P3 terminated"<<endl;

       pid = wait(NULL);
       if(pid == p2)
        cout<<"P2 terminated"<<endl;
       else if(pid == p3)
        cout<<"P3 terminated"<<endl;
    }
}