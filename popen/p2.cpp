#include "../cn.h"

int main() {
    FILE *fp = popen("./p3.exe","w");
    int fd = fileno(fp);
    dup2(fd,1);
    string buff;
    while(1) {
        getline(cin,buff);
        cout<<buff<<endl;    
    }    
}