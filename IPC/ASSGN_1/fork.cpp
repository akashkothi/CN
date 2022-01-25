#include "../cn.h"

int main(){
    pid_t p2, p3, p4;
    cout<<"Process P1 with id "<<getpid()<<" started executing"<<endl;
    if((p2 = fork()) < 0)
        cout<<"fork error"<<endl;
    else if(p2 == 0) {

        cout<<"Process P2 is created by parent process "<<getppid()<<endl;
        cout<<"Process P2 with id "<<getpid()<<" started executing"<<endl;
        
        if((p3 = fork()) < 0)
            cout<<"fork error"<<endl;
        else if(p3 == 0) {

            cout<<"Process P3 is created by parent process "<<getppid()<<endl;
            cout<<"Process P3 with id "<<getpid()<<" started executing"<<endl;

            if((p4 = fork()) < 0)
                cout<<"fork error"<<endl;
            else if(p4 == 0){

                cout<<"Process P4 is created by parent process "<<getppid()<<endl;
                cout<<"Process P4 with id "<<getpid()<<" started executing"<<endl;

                cout<<"Process P4("<<getpid()<<") completed executing"<<endl;    
            }
            else{
                wait(NULL);
                cout<<"Process P3("<<getpid()<<") completed executing"<<endl;    
            }
        }
        else{
            wait(NULL);
            cout<<"Process P2("<<getpid()<<") completed executing"<<endl;    
        }
    }
    else{
        wait(NULL);
        cout<<"Process P1("<<getpid()<<") completed executing"<<endl;    
    }
    return 0;
}