#include <bits/stdc++.h>
#include "ace/Log_Msg.h"
#include "ace/OS_NS_errno.h"
#include "ace/OS_NS_unistd.h"
#include "ace/OS_NS_stdlib.h"
#include "ace/Shared_Memory_MM.h"
#include "ace/Process_Semaphore.h"

ACE_TCHAR shm_key1[] = ACE_TEXT ("/tmp/X");
ACE_TCHAR shm_key2[] = ACE_TEXT ("/tmp/Y");

#define SHM_SIZE 10

int ACE_TMAIN (int , ACE_TCHAR *[]) {    

    ACE_Process_Semaphore s1 (0, ACE_TEXT("S1"));
    ACE_Process_Semaphore s2 (0, ACE_TEXT("S2"));

    ACE_Shared_Memory *X = new ACE_Shared_Memory_MM(shm_key1,SHM_SIZE);
    ACE_Shared_Memory *Y = new ACE_Shared_Memory_MM(shm_key2,SHM_SIZE);

    int *x = (int *) X->malloc();
    int *y = (int *) Y->malloc();

    *x = *y = 1;

    while(1) {
        ACE_DEBUG ((LM_DEBUG,"Reading from shm Y ...\n"));
        ACE_DEBUG ((LM_DEBUG, "Value of y read from shared memory Y : %d \n",*y));
        *x  = *y + 1;
        ACE_DEBUG ((LM_DEBUG, "Value of x written to shared memory X : %d \n",*x));
        ACE_OS::sleep (3);
        s1.release();
        ACE_DEBUG ((LM_DEBUG, "Waiting for P2 ...\n",*x));
        s2.acquire();
    }

}