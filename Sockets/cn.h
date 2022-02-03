#include <bits/stdc++.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>
#include <poll.h>
#include <ctype.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFSIZE 1024
#define RWX 0666
#define R O_RDONLY
#define W O_WRONLY
#define RW O_RDWR
#define PORT 8080
#define BACKLOG 10
#define LOCAL_HOST "127.0.0.1"

int ports[3] = {8081,8082,8083};

struct mymesg {
    long type;
    char text[BUFFSIZE];
} msg;

using namespace std;

void error(const char* err) {
    perror(err);
    exit(EXIT_FAILURE);
}

void capitalize(char *buff) {
    int n = strlen(buff);
    for(int i = 0; i < n; i++)
        buff[i] = toupper(buff[i]);
}

void de_capitalize(char *buff) {
    int n = strlen(buff);
    for(int i = 0; i < n; i++)
        buff[i] = tolower(buff[i]);
}

void init_socket_address(struct sockaddr_in* socket_addr, const char* ip_addr, int port) {
    socket_addr->sin_family = AF_INET;
    socket_addr->sin_port = htons(port);
    if(inet_pton(AF_INET,ip_addr,&socket_addr->sin_addr) < 1)
        error("inet_pton error");
    printf("\nInitialized socket address ...\n");
}