#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>

#include <syslog.h>
#include <sys/time.h>

#define NUM_THREADS 128

void *sumIdx(void *threadp);

typedef struct
{
    int threadIdx;
}threadParams_t;

pthread_t threads[NUM_THREADS];
threadParams_t threadParams[NUM_THREADS];

void *sumOfIdx(void *threadp)
{
    threadParams_t *iThreadp = (threadParams_t *)threadp;
    int sum = (iThreadp->threadIdx * (iThreadp->threadIdx + 1)) / 2;
    
    syslog(LOG_INFO, "Thread idx=%d, sum[1...%d]=%d", 
                    iThreadp->threadIdx, 
                    iThreadp->threadIdx,
                    sum
                    );  
}


int main(int argc, char *argv[])
{
    FILE *pipe;
    char unameMsg[250];

    openlog("[COURSE:1][ASSIGNMENT:2]", LOG_NDELAY, LOG_USER);

    pipe = popen("uname -a", "r");

    if(pipe)
    {
        fgets(unameMsg, sizeof(unameMsg), pipe);
        syslog(LOG_INFO, "%s", unameMsg);
    }

    for(int i = 0; i < NUM_THREADS; i++)
    {
        threadParams[i].threadIdx = i;

        pthread_create(&threads[i],
                        (void *)0,
                        sumOfIdx,
                        (void *)&threadParams[i]
                        );
    }

    for(int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i] ,NULL);
    }

    closelog();
}