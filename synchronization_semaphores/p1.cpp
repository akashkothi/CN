#include "../cn.h"

int main() {

    char ch;
    sem_unlink("s12");
    sem_unlink("s41");
    sem_t *s12 = sem_open("s12",O_CREAT|O_EXCL,RWX,0);
    sem_t *s41 = sem_open("s41",O_CREAT|O_EXCL,RWX,0);


    cout<<"I am process P1 ..."<<endl;
    cout<<"Enter any character to signal(S12) : ";
    cin>>ch;
    
    cout<<"I am signalling semaphore signal of S12 ..."<<endl;
    
    sem_post(s12);
    
    cout<<"I am waiting for semaphore signal S41 ..."<<endl;
    
    sem_wait(s41);
    
    cout<<"I got signal from process P4 ..."<<endl;
}