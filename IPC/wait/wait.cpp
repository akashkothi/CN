#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
using namespace std;

int main() {

    pid_t p2, p3;

    if((p2 = fork()) < 0)
        perror("fork error");
    else if(p2 > 0) {

        if((p3 = fork()) < 0)
            perror("fork error");
        else if(p3 > 0) {

            cout<<"In parent ..."<<endl;

            pid_t pid = wait(NULL);

            if(pid == p2)
                cout<<"P2 terminated ..."<<endl;
            else if(pid == p3)
                cout<<"P3 terminated ..."<<endl;

            pid = wait(NULL);
            if(pid == p2)
                cout<<"P2 terminated ..."<<endl;
            else if(pid == p3)
                cout<<"P3 terminated ..."<<endl;
        
        }
        else {
            // sleep(2);
            sleep(3);
            execl("./p3.exe","./p3.exe",NULL);
        }
    }
    else {
        sleep(2);
        // sleep(3);
        execl("./p2.exe","./p2.exe",NULL);
    }
}