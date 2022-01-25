#include "../cn.h"

int main() {

    char ch;
    sem_unlink("s23");
    sem_t *s12 = sem_open("s12",0);
    sem_t *s23 = sem_open("s23",O_CREAT|O_EXCL,RWX,0);

    cout<<"I am process P2 ..."<<endl;
    cout<<"I am waiting for semaphore signal S12 ..."<<endl;
    
    sem_wait(s12);
    
    cout<<"I got semaphore S12 signalling from process P1 ... "<<endl;
    cout<<"Enter any character to signal(S23) : ";
    cin>>ch;
    
    cout<<"I am signalling semaphore signal of S23 ..."<<endl;
    
    sem_post(s23);
}