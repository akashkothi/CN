#include "../cn.h"

int main() {

    char ch;
    sem_unlink("s34");
    sem_t *s23 = sem_open("s23",0);
    sem_t *s34 = sem_open("s34",O_CREAT|O_EXCL,RWX,0);

    cout<<"I am process P3 ..."<<endl;
    cout<<"I am waiting for semaphore signal S23 ..."<<endl;
    
    sem_wait(s23);
    
    cout<<"I got semaphore S23 signalling from process P2 ... "<<endl;
    cout<<"Enter any character to signal(S34) : ";
    cin>>ch;
    
    cout<<"I am signalling semaphore signal of S34 ..."<<endl;
    
    sem_post(s34);
    
}