#include "../cn.h"

char r_buff[BUFFSIZE];
char w_buff[BUFFSIZE];
int fd1[2],fd2[2];

char* to_str(int n){
    char* s = (char *)malloc(10);
    string num = to_string(n);
    for(int i = 0; i < num.size(); i++)
        s[i] = num[i];
    return s;
}

void* read_(void *arg) {
    while(1){
        read(fd2[0],r_buff,BUFFSIZE);
        cout<<"Child(In Parent) : "<<r_buff<<endl;
    }
}

void* write_(void *arg) {
    while(1) {
        cin.getline(w_buff,BUFFSIZE);
        write(fd1[1],w_buff,BUFFSIZE);        
    }
}

int main(){

    pid_t pid;
    pthread_t reader, writer;

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
        execl("./thread_pipe2_child.exe","./thread_pipe2_child.exe",to_str(keyboard),to_str(screen),(char*)NULL);
    else {

        dup2(keyboard,STDIN_FILENO);
        dup2(screen,STDOUT_FILENO);
        
        pthread_create(&reader,NULL,read_,NULL);
        pthread_create(&writer,NULL,write_,NULL);
        pthread_join(reader,NULL);
        pthread_join(writer,NULL);

    }
}