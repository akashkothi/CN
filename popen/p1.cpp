#include "../cn.h"

int fd[2];
char r_buff[BUFFSIZE];
string buff;

void* read_(void *arg) {
    while(1) {
        read(fd[0],r_buff, BUFFSIZE);
        cout<<r_buff<<endl;
    }
}

void* write_(void *arg) {
    while(1) {
        getline(cin,buff);
        buff.push_back('\n');
        write(fd[1],buff.c_str(),buff.size());        
    }
}


int main() {

    pthread_t reader, writer;

    mkfifo("./fifo",RWX);
    fd[0] = open("./fifo",O_RDWR);

    FILE *fp = popen("./p2.exe","w");
    fd[1] = fileno(fp);
    
    pthread_create(&reader,NULL,read_,NULL);
    pthread_create(&writer,NULL,write_,NULL);
    pthread_join(reader,NULL);
    pthread_join(writer,NULL);

}