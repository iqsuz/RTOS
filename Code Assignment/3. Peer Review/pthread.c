#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>

#include <syslog.h>
#include <sys/time.h>

#define COUNT 1000  //Count up to
#define NUM_THREADS 2   //Thread numbers
#define INC_THREAD 0    //Thread index for increment func..
#define DEC_THREAD 1    //Thread index for decrement func..

//Function prototypes.
void *incThread(void *);
void *decThread(void *);

//Declaration of thread input struct
typedef struct
{
    int threadIdx;
}threadParams_t;

pthread_t threads[NUM_THREADS];     //Thread handler
threadParams_t threadParams[NUM_THREADS];   //Definition of thread input struct

int gsum = 0;   //Unsafe global variable for summation

int main(int argc, char *argv)
{
    threadParams[INC_THREAD].threadIdx = INC_THREAD;
    threadParams[DEC_THREAD].threadIdx = DEC_THREAD;

    pthread_create(&threads[INC_THREAD],
                    (void *)0,
                    incThread,
                    (void *)&threadParams[INC_THREAD]
                    );

    pthread_create(&threads[DEC_THREAD],
                    (void *)0,
                    decThread,
                    (void *)&threadParams[DEC_THREAD]
                    );

    for(int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    printf("TEST COMPLETED\n");
}

/*
Function decleration of decrement thread. 
This function decrement gsum variable by i of for loop.
*/
void *decThread(void *threadp)
{
    threadParams_t *iThread = (threadParams_t *)threadp;    //Type casting from void address to threadPAram_t address.

    for(int i = 0; i < COUNT; i++)
    {
        gsum -= i;
        printf("Decrement thread idx=%d, gsum=%d\n", iThread->threadIdx, gsum);
    }
}

/*
Function decleration of increment thread. 
This function increment gsum variable by i of for loop.
*/
void *incThread(void *threadp)
{
    threadParams_t *iThread = (threadParams_t *)threadp;    //Type casting from void address to threadParams_t address.

    for (int  i = 0; i < COUNT; i++)
    {
        gsum += i;
        printf("Increment thread idx=%d, gsum=%d\n", iThread->threadIdx, gsum);
    }
    
}
