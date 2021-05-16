#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>

#include <syslog.h>
#include <sys/time.h>

#define NUM_THREADS 1   //number of thread.
#define HTHREAD_ID 0    //Thread ID for hello world thread.

void *helloWorldFromThread(void *);     //Declaration of function prototype.

//Declaration of thread input param struct.
typedef struct
{
    int threadIdx;
}threadParams_t;


pthread_t threads[NUM_THREADS];     //Thread handler.
threadParams_t threadParams[NUM_THREADS];   //Definition of input param.

int main(int argc, char *argv[])
{
    openlog("[COURSE:1][ASSIGNMENT:1]", LOG_NDELAY, LOG_USER);      //Sys header setup.
    
    FILE *pipe;     //Declaration of file for pipe.
    char unameMsg[250]; //char array for return string.

    pipe = popen("uname -a", "r");      //Execute uname -a.

    if(pipe)        //Successfull?
    {
        fgets(unameMsg, sizeof(unameMsg), pipe);     //store return value in unameMsg.
        syslog(LOG_INFO, "%s",unameMsg);    //push it to syslog.
    }

    syslog(LOG_INFO, "Hello World from Main!");     //message register from main thread.

    threadParams[HTHREAD_ID].threadIdx = HTHREAD_ID;    //set thread id as value of HTREAD_ID which is 0.

    //create thread 
    pthread_create(&threads[HTHREAD_ID], 
                    (void *)0,
                    helloWorldFromThread,
                    (void *)&(threadParams[HTHREAD_ID])
                    );

    pthread_join(threads[HTHREAD_ID] ,NULL);        //Run the thread.

    closelog();     //close syslog file.
    
    printf("TEST COMPLETED!\n");
}

void *helloWorldFromThread(void *threadp)
{
    syslog(LOG_INFO, "Hello World from Thread!");   //push message to syslog.
}
