#include "../cn.h"

char buff[BUFFSIZE];

void uppercase(char *s){
    int n = strlen(s);
    for(int i = 0; i < n; ++i)
        s[i] = toupper(s[i]);
    return;
}

int main(){

    pid_t pid;
    int fd1[2],fd2[2];

    if(pipe(fd1) < 0)
        perror("pipe error");
    if(pipe(fd2) < 0)
        perror("pipe error");

    if((pid = fork()) < 0)
        perror("fork error");
    else if(pid == 0) {
        
        close(fd1[1]);
        close(fd2[0]);

        while(1) {
            read(fd1[0],buff,BUFFSIZE);
            cout<<"Child reading from parent : "<<buff<<endl;
            uppercase(buff);
            cout<<"Child writing to parent.... \n";
            write(fd2[1],buff,BUFFSIZE);
        }

    }
    else {
        
        close(fd1[0]);
        close(fd2[1]);

        while(1) {
            cout<<"Parent writing to child : ";
            cin.getline(buff,BUFFSIZE);
            write(fd1[1],buff,BUFFSIZE);
            read(fd2[0],buff,BUFFSIZE);
            cout<<"Parent reading from child : "<<buff<<endl;
        }

    }

}