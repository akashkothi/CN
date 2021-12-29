#include "../cn.h"

int main() {

    char buff[BUFFSIZE];
    read(0,buff,BUFFSIZE);
    cout<<"P2 : "<<buff<<endl;
    FILE *fp = popen("./popen_3.exe","w");
    int fd = fileno(fp);
    write(fd,buff,BUFFSIZE);
    wait(NULL);
}