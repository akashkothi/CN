#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
using namespace std;

int main() {
    pid_t p2;
    cout<<"Parent started executing ..."<<endl;
    if((p2 = fork()) < 0)
        perror("fork error");
    else if(p2 > 0) {
        // sleep(1);
        cout<<"In parent ..."<<endl;
        wait(NULL);
        cout<<"Parent terminated ..."<<endl;
    }
    else {
        cout<<"Child started executing ..."<<endl;
        // sleep(1);
        execl("./p2.exe","./p2.exe",NULL);
        cout<<"Child terminated ..."<<endl;
    }
}