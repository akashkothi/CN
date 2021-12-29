#include "../cn.h"

int main() {

    char buff[BUFFSIZE];
    FILE *fp = popen("./popen_2.exe","w");
    int fd = fileno(fp);
    cout<<"P1 : ";
    cin>>buff;
    write(fd,buff,BUFFSIZE);
    wait(NULL);

}