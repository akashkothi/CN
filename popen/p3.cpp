#include "../cn.h"

void capitalize(string &s) {
    if(s.size() == 0) return;
    s[0] = toupper(s[0]);
    for(int i = 1; i < s.length(); i++)
        s[i] = tolower(s[i]);
}

int main() {

    string buff;
    mkfifo("./fifo",RWX);
    int fd = open("./fifo",O_RDWR);

    while(1) {
        getline(cin, buff);
        capitalize(buff);
        buff = "Message from P3 : " + buff;
        write(fd,buff.c_str(),buff.size());
    }

}