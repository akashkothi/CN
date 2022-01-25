#include "../cn.h"
char buff[1];

void capitalize(string &s){
    if(s.size() == 0) return;
    s[0] = toupper(s[0]);
    for(int i = 1; i < s.length(); i++)
        s[i] = tolower(s[i]);
}

int main(){
    int fd1, fd2;
    string word = "";
    fd1 = open("./capitalize.txt",R);
    fd2 = open("./capitalized_result.txt",W);

    while(read(fd1,buff,1) > 0) {
        if((*buff == ' '||*buff == '\n')){
            capitalize(word);
            char const *str = word.c_str();
            write(fd2,str,strlen(str));
            write(fd2," ",1);
            word.erase();
        }
        else word.push_back(*buff);
    }

    if(word.size() > 0){
        capitalize(word);
        char const *str = word.c_str();
        write(fd2,str,strlen(str));
        write(fd2," ",1);
        word.erase();
    }

}