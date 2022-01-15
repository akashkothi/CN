#include "../cn.h"

int main() {

    char ch;
    sem_t *s34 = sem_open("s34",0);
    sem_t *s41 = sem_open("s41",0);

    cout<<"I am process P4 ..."<<endl;
    cout<<"I am waiting for semaphore signal S34 ..."<<endl;
    
    sem_wait(s34);
    
    cout<<"I got semaphore S34 signalling from process P3 ... "<<endl;
    cout<<"Enter any character to signal(S41) : ";
    cin>>ch;
    
    cout<<"I am signalling semaphore signal of S41 ..."<<endl;
    
    sem_post(s41);
    
}