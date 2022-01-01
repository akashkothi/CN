#include "../cn.h"

int fd[2];
char r_buff[BUFFSIZE];
char w_buff[BUFFSIZE];

void* read_(void *arg) {
    while(1){
        read(fd[0],r_buff,BUFFSIZE);
        cout<<r_buff<<endl;
    }
}

void* write_(void *arg) {
    while(1) {
        cin.getline(w_buff,BUFFSIZE);
        write(fd[1],w_buff,BUFFSIZE);        
    }
}

int main() {

    pthread_t reader, writer;

    mkfifo("c3_write_fifo",RWX);
    mkfifo("c3_read_fifo",RWX);
    fd[1] = open("c3_write_fifo",W);
    fd[0] = open("c3_read_fifo",R);

    pthread_create(&reader,NULL,read_,NULL);
    pthread_create(&writer,NULL,write_,NULL);
    pthread_join(reader,NULL);
    pthread_join(writer,NULL);

}