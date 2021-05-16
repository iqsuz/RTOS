#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>

#include <syslog.h>
#include <sys/time.h>

#define NUM_THREADS 128 //Number of threads

void *sumIdx(void *threadp);    //Function prototype

typedef struct      //Thread input struct decleration.
{
    int threadIdx;
}threadParams_t;

pthread_t threads[NUM_THREADS];     //Thread handler.
threadParams_t threadParams[NUM_THREADS];      //Thread input struct defination.

int main(int argc, char *argv[])
{
    FILE *pipe;     //uname pipe file.
    char unameMsg[250];     //string for uname result.

    openlog("[COURSE:1][ASSIGNMENT:2]", LOG_NDELAY, LOG_USER);  //open syslog with assigned header.

    pipe = popen("uname -a", "r");     //run bash code.

    if(pipe)
    {
        fgets(unameMsg, sizeof(unameMsg), pipe);    //get the string.
        syslog(LOG_INFO, "%s", unameMsg);   //log it.
    }

    for(int i = 0; i < NUM_THREADS; i++)
    {
        threadParams[i].threadIdx = i;  //Assign thread id to its input param.

        //create thread with following parameters.
        pthread_create(&threads[i],
                        (void *)0,
                        sumOfIdx,
                        (void *)&threadParams[i]
                        );
    }

    for(int i = 0; i < NUM_THREADS; i++)    //Run threads.
    {
        pthread_join(threads[i] ,NULL);
    }

    closelog();     //close syslog file.
}

void *sumOfIdx(void *threadp)
{
    threadParams_t *iThreadp = (threadParams_t *)threadp;       //Type casting for input param.
    int sum = (iThreadp->threadIdx * (iThreadp->threadIdx + 1)) / 2;        //Calculate the summation.
    
    syslog(LOG_INFO, "Thread idx=%d, sum[1...%d]=%d", //log it.
                    iThreadp->threadIdx, 
                    iThreadp->threadIdx,
                    sum
                    );  
}