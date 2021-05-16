#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>

#include <syslog.h>
#include <sys/time.h>

#define NUM_THREADS 1

void *helloWorldFromThread(void *);

typedef struct
{
    int threadIdx;
}threadParams_t;


pthread_t threads[NUM_THREADS];
threadParams_t threadParams[NUM_THREADS];

int main(int argc, char *argv[])
{
    openlog("[COURSE:1][ASSIGNMENT:1]", LOG_NDELAY, LOG_USER);
    
    FILE *pipe;
    char unameMsg[250];

    pipe = popen("uname -a", "r");

    if(pipe)
    {
        fgets(unameMsg, 250, pipe);
        syslog(LOG_INFO, "%s",unameMsg);
    }

    syslog(LOG_INFO, "Hello World from Main!");

    for(int i = 0; i < NUM_THREADS; i++)
    {
        threadParams[i].threadIdx = i;

        pthread_create(&threads[i], 
                        (void *)0,
                        helloWorldFromThread,
                        (void *)&(threadParams[i])
                        );
    }

    for(int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i] ,NULL);
    }

    closelog();
}

void *helloWorldFromThread(void *threadp)
{
    syslog(LOG_INFO, "Hello World from Thread!");
}