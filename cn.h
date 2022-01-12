#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <pthread.h>
#include <poll.h>
#include <ctype.h>
#include <signal.h>


#define BUFFSIZE 1024
#define RWX 0666
#define R O_RDONLY
#define W O_WRONLY
#define RW O_RDWR
struct mymesg {
    long type;
    char text[BUFFSIZE];
} msg;

using namespace std;