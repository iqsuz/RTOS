#define _GNU_SOURCE
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sched.h>
#include <time.h>
#include <syslog.h>

#include <sys/sysinfo.h>
#include <sys/types.h>
#include <sys/time.h>

#include <unistd.h>

#define NUM_THREADS 128
#define THREAD_CPU (3)

//Decleration of thread input struct.
typedef struct
{
    int threadIdx;
}threadParams_t;

void print_scheduler(void); //Prints the scheduler for thread.
void *sumOfIdx(void *);     //Function that will be called when thread is being executed.
void syslog_OSInfo(void);   //Push uname -a output to syslog.

int main(int argc, char *argv[])
{
    int rc;
    int cpu_id = THREAD_CPU;    //Desired CPU core that we want to work with.

    struct sched_param rt_param;    

    pthread_t threads[NUM_THREADS];     //Thread handlers.
    pthread_attr_t rt_sched_attr;       //Thread attr handler. This variable will be used to change scheduler during the thread creation.
    cpu_set_t assigned_cpu;             //CPU set handler.
    threadParams_t threadParams[NUM_THREADS];   //Defination of thread input parameter.
    
    syslog_OSInfo();        //Push OS info to syslog as required in assignment.
    printf("INITIAL: ");
    print_scheduler();      //Print current scheduler that main thread runs on.

    //Set desired CPU core as thread processor.
    CPU_ZERO(&assigned_cpu);
    CPU_SET(cpu_id, &assigned_cpu);

    //Set SCHED policy to SCHED_FIFO and set affinity for desired CPU core.
    rc = pthread_attr_init(&rt_sched_attr);
    rc = pthread_attr_setinheritsched(&rt_sched_attr, PTHREAD_EXPLICIT_SCHED);
    rc = pthread_attr_setschedpolicy(&rt_sched_attr, SCHED_FIFO);
    rc = pthread_attr_setaffinity_np(&rt_sched_attr, sizeof(assigned_cpu), &assigned_cpu);  //CPU core has been chosen.

    rt_param.sched_priority = sched_get_priority_max(SCHED_FIFO);   //Set thread priority as MAX.

    pthread_attr_setschedparam(&rt_sched_attr, &rt_param);      //Set the scheduler policy.
    //sched_setscheduler(getpid(), SCHED_FIFO, &rt_param);

    printf("ADJUSTED: ");
    print_scheduler();

    for(int i = 1; i < NUM_THREADS; i++)    //Create threads.
    {
       threadParams[i].threadIdx = i;
       pthread_create(&threads[i], &rt_sched_attr, sumOfIdx, (void *)&threadParams[i]);
    }

    for(int i = 1; i < NUM_THREADS; i++)    //Wait until all threads are executed.
    {
        pthread_join(threads[i], NULL);
    }

    printf("TEST COMPLETED!\n");

}

//This function print out current scheduler.
void print_scheduler(void)
{
    int schedType;

    int self_sched_pol;
    struct sched_param self_sched_param;

    //schedType = sched_getscheduler(getpid());   //Which scheduler policy on duty?
    pthread_getschedparam(pthread_self(), &self_sched_pol, &self_sched_param);

    switch(self_sched_pol)
    {
        case SCHED_OTHER:
            printf("Thread scheduler policy is SCHED_OTHER.\n");
            break;

        case SCHED_FIFO:
            printf("Thread scheduler policy is SCHED_FIFO.\n");
            break;
        
        case SCHED_RR:
            printf("Thread scheduler policy is SCHED_RR.\n");
            break;
        
        default:
            printf("Thread scheduker policy is UNKNOWN.\n");

    }

}


void *sumOfIdx(void *threadp)
{
    threadParams_t *threadParam = (threadParams_t *)threadp;    //Type casting since it is passed as void *.
    int sum = threadParam->threadIdx * (threadParam->threadIdx + 1) / 2;    //Summation with Gauss formula.

    print_scheduler();

    printf("Thread idx=%d, sum[1...%d]=%d Running on core: %d\n", //log it.
                    threadParam->threadIdx,
                    threadParam->threadIdx,
                    sum,
                    sched_getcpu()
                    ); 
}

void syslog_OSInfo(void)
{
    FILE *pipe;     //uname pipe file.
    char unameMsg[250];     //string for uname result.

    openlog("[COURSE:1][ASSIGNMENT:3]", LOG_NDELAY, LOG_USER);  //open syslog with assigned header.

    pipe = popen("uname -a", "r");     //run bash code.

    if(pipe)
    {
        fgets(unameMsg, sizeof(unameMsg), pipe);    //get the string.
        syslog(LOG_INFO, "%s", unameMsg);   //log it.
    }
}
