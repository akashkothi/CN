#include "../cn.h"
char buff[1];

int main() {
    off_t num_offset, str_offset;
    int fd1, fd2;
    
    fd1 = open("./text.txt",R);
    fd2 = open("./separated_text.txt",W);

    num_offset = lseek(fd2,0,SEEK_CUR);
    str_offset = lseek(fd2,80,SEEK_SET);

    while(read(fd1,buff,1) > 0) {
        if(isalpha(*buff)) {
            str_offset = lseek(fd2,str_offset+1,SEEK_SET);
            if((write(fd2,buff,1)) == -1)
                perror("write error"); 
        }
        else if(isdigit(*buff)) {
            num_offset = lseek(fd2,num_offset+1,SEEK_SET);
            if((write(fd2,buff,1)) == -1)
                perror("write error");
        }
    }
}