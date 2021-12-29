#include "../cn.h"

char buff[BUFFSIZE];

void uppercase(char *s) {
    int n = strlen(s);
    for (int i = 0; i < n; ++i)
        s[i] = toupper(s[i]);
    return;
}

int main(int argc, const char *argv[]) {
    int fd[2];
    fd[0] = dup(STDIN_FILENO);
    fd[1] = dup(STDOUT_FILENO);
    dup2(atoi(argv[1]),STDIN_FILENO);
    dup2(atoi(argv[2]),STDOUT_FILENO);
    
    while(1) {
        read(fd[0],buff, BUFFSIZE);
        cout<<"Message received from parent..."<<endl;
        uppercase(buff);
        write(fd[1],buff, BUFFSIZE);
        cout<<"Message sent to parent : "<<buff<<endl;
    }
}