#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
 
int main() {
	int fd, c;
	fd = open("/dev/keyled",O_RDWR);    
    for(int i = 0; i < 3; i++){
        char buff[4] = {'\0'};
        printf("Enter the number : ");
        scanf("%s", buff);
        write(fd,buff,strlen(buff));    
    }   
	close(fd);
}