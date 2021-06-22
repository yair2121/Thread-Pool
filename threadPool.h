//Yair Yariv Yardeni 315009969
#ifndef __THREAD_POOL__
#define __THREAD_POOL__

#include "osqueue.h"

typedef enum { // For readability- FALSE is 0 and TRUE is 1.
    FALSE = 0, TRUE
} bool;

typedef void (*taskFunc) (void *);

typedef struct thread_pool
{
    OSQueue tasks; // taskFunc queue
    OSQueue thread_queue;
    size_t pool_size;
}ThreadPool;

ThreadPool* tpCreate(int numOfThreads);

void tpDestroy(ThreadPool* threadPool, int shouldWaitForTasks);

int tpInsertTask(ThreadPool* threadPool, void (*computeFunc) (void *), void* param);
#endif
