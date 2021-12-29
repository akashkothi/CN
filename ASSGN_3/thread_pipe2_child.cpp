#include "../cn.h"

char r_buff[BUFFSIZE];
char w_buff[BUFFSIZE];
int fd[2];

void* read_(void *arg) {

    while(1){
        read(fd[0],r_buff, BUFFSIZE);
        cout<<"Parent(In Chlid) : "<<r_buff<<endl;
    }

}

void* write_(void *arg) {
     
    while(1) {
        cin.getline(w_buff,BUFFSIZE);
        write(fd[1],w_buff, BUFFSIZE);       
    }
     
}

int main(int argc, const char *argv[]) {
    
    pthread_t reader, writer;

    fd[0] = dup(STDIN_FILENO);
    fd[1] = dup(STDOUT_FILENO);
    dup2(atoi(argv[1]),STDIN_FILENO);
    dup2(atoi(argv[2]),STDOUT_FILENO);
    
    pthread_create(&reader,NULL,read_,NULL);
    pthread_create(&writer,NULL,write_,NULL);
    pthread_join(reader,NULL);
    pthread_join(writer,NULL);

}