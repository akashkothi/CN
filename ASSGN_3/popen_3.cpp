#include "../cn.h"

int main() {

    char buff[BUFFSIZE];
    read(0,buff,BUFFSIZE);
    cout<<"P3 : "<<buff<<endl;
    FILE *fp = popen("./popen_4.exe","w");
    int fd = fileno(fp);
    write(fd,buff,BUFFSIZE);
    wait(NULL);

}