#include "../cn.h"

char buff[BUFFSIZE];

char* to_str(int n){
    char* s = (char *)malloc(10);
    string num = to_string(n);
    for(int i = 0; i < num.size(); i++)
        s[i] = num[i];
    return s;
}

int main(){

    pid_t pid;
    int fd1[2],fd2[2];

    if(pipe(fd1) < 0)
        perror("pipe error");
    if(pipe(fd2) < 0)
        perror("pipe error");

    int keyboard = dup(STDIN_FILENO);
    int screen = dup(STDOUT_FILENO);  
    dup2(fd1[0],0);
    close(fd1[0]);
    dup2(fd2[1],1);
    close(fd2[1]);

    if((pid = fork()) < 0)
        perror("fork error");
    else if(pid == 0)
        execl("./pipe2_child.exe","./pipe2_child.exe",to_str(keyboard),to_str(screen),(char*)NULL);
    else {

        dup2(keyboard,STDIN_FILENO);
        dup2(screen,STDOUT_FILENO);

        while(1) {
            cout<<"Parent writing to child : ";
            cin.getline(buff,BUFFSIZE);
            write(fd1[1],buff,BUFFSIZE);
            read(fd2[0],buff,BUFFSIZE);
            cout<<"Parent reading from child : "<<buff<<endl;
        }
    }
}