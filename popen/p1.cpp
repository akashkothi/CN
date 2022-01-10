#include "../cn.h"

int main() {

    string buff;
    FILE *fp = popen("./p2.exe","w");
    int fd = fileno(fp);
    while(1) {
        getline(cin, buff);
        buff.push_back('\n');
        write(fd,buff.c_str(),buff.size());
    }

}