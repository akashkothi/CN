#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
 
#define BUFFER_LENGTH 256               ///< The buffer length (crude but fine)
static char receive[BUFFER_LENGTH];     ///< The receive buffer from the LKM
 
int main() {
	int fd, c;
	fd = open("/dev/mydev1",O_RDWR); // opens the device driver which has been loaded
	printf("%d\n",fd);
	
	char buff1[100] = {'\0'};
	c = read(fd,buff1,sizeof(100)); // reads the time of the system through device driver
	printf("%d out read %s\n",c,buff1); // prints the system time in seconds(nu)
    
	char buff2[100] = {'\0'};
	scanf("%s",buff2);  // takes input from user to which current time has to be set
	c = write(fd,buff2,strlen(buff2)); // writes the time this changes the system time
	printf("%d out write \n",c); // prints size of char buffer
    
	close(fd);
}