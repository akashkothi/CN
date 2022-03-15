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
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/un.h>

#define BUFFSIZE 1024
#define RWX 0666
#define R O_RDONLY
#define W O_WRONLY
#define RW O_RDWR
#define PORT 8080
#define BACKLOG 10
#define LOCAL_HOST "127.0.0.1"
#define ADDRESS "mysocket"

int ports[3] = {8081,8082,8083};
const char* name[3] = {"mysocket1","mysocket2","mysocket3"};

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
    // printf("\nInitialized socket address ...\n");
}

pid_t getpid_by_name(string name) {
    
    int fd;
    pid_t pid;
    char buff[BUFFSIZE] = {'\0'};
    
    name = "pidof " + name;
    fd = fileno(popen(name.c_str(),"r"));
    read(fd,buff,BUFFSIZE);
    pid = atoi(buff);
    
    return pid;
}

int send_fd(int socket, int fd_to_send) {
    
    struct msghdr socket_message;
    struct iovec io_vector[1];
    struct cmsghdr *control_message = NULL;
    char message_buffer[1];
    
    
    /* storage space needed for an ancillary element with a paylod of
    length is CMSG_SPACE(sizeof(length)) */
    char ancillary_element_buffer[CMSG_SPACE(sizeof(int))];
    int available_ancillary_element_buffer_space;
    
    
    /* at least one vector of one byte must be sent */
    message_buffer[0] = 'F';
    io_vector[0].iov_base = message_buffer;
    io_vector[0].iov_len = 1;
    
    
    /* initialize socket message */
    memset(&socket_message, 0, sizeof(struct msghdr));
    socket_message.msg_iov = io_vector;
    socket_message.msg_iovlen = 1;
    
    
    /* provide space for the ancillary data */
    available_ancillary_element_buffer_space = CMSG_SPACE(sizeof(int));
    memset(ancillary_element_buffer, 0, available_ancillary_element_buffer_space);
    socket_message.msg_control = ancillary_element_buffer;
    socket_message.msg_controllen = available_ancillary_element_buffer_space;


    /* initialize a single ancillary data element for fd passing */
    control_message = CMSG_FIRSTHDR(&socket_message);
    control_message->cmsg_level = SOL_SOCKET;
    control_message->cmsg_type = SCM_RIGHTS;
    control_message->cmsg_len = CMSG_LEN(sizeof(fd_to_send));
    *((int *) CMSG_DATA(control_message)) = fd_to_send;
    
    return sendmsg(socket, &socket_message, 0);
}

int recv_fd(int socket) {
    
    int sent_fd, available_ancillary_element_buffer_space;
    struct msghdr socket_message;
    struct iovec io_vector[1];
    struct cmsghdr *control_message = NULL;
    char message_buffer[1];
    char ancillary_element_buffer[CMSG_SPACE(sizeof(int))];
    
    /* start clean */
    memset(&socket_message, 0, sizeof(struct msghdr));
    memset(ancillary_element_buffer, 0, CMSG_SPACE(sizeof(int)));
    
    /* setup a place to fill in message contents */
    io_vector[0].iov_base = message_buffer;
    io_vector[0].iov_len = 1;
    socket_message.msg_iov = io_vector;
    socket_message.msg_iovlen = 1;
    
    /* provide space for the ancillary data */
    socket_message.msg_control = ancillary_element_buffer;
    socket_message.msg_controllen = CMSG_SPACE(sizeof(int));
    
    if(recvmsg(socket, &socket_message, MSG_CMSG_CLOEXEC) < 0) {
        return -1;
    }
    
    if(message_buffer[0] != 'F') {
        /* this did not originate from the above function */
        return -1;
    }

    if((socket_message.msg_flags & MSG_CTRUNC) == MSG_CTRUNC) {
        /* we did not provide enough space for the ancillary element array */
        return -1;
    }
    
    /* iterate ancillary elements */
    for(control_message = CMSG_FIRSTHDR(&socket_message); control_message != NULL; control_message = CMSG_NXTHDR(&socket_message, control_message)) {
        if((control_message->cmsg_level == SOL_SOCKET) &&(control_message->cmsg_type == SCM_RIGHTS)) {
            sent_fd = *((int *) CMSG_DATA(control_message));
            return sent_fd;
        }
    }

    return -1; 
}


int uds_socket(int type, const char* name,struct sockaddr_un &userv_addr ) {

    int usfd;
    
    if((usfd = socket(AF_UNIX, type, 0)) < 0)
        error("socket error");
    
    bzero(&userv_addr,sizeof(userv_addr));
    userv_addr.sun_family = AF_UNIX;
    strcpy(userv_addr.sun_path, name);

    return usfd;
}