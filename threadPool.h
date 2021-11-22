//Yair Yariv Yardeni
#ifndef __THREAD_POOL__
#define __THREAD_POOL__

#include "osqueue.h"
#include "stdlib.h"
#include <pthread.h>
#include <stdio.h>

typedef enum { // For readability- FALSE is 0 and TRUE is 1.
    FALSE = 0, TRUE
} bool;

#define SYSTEM_CALL_ERROR "Error in system call"

typedef enum {
    UNSUCCESSFUL  = -1,
    SUCCESSFUL = 0
} functionState;

typedef enum {
    CREATING = 0,
    RUNNING,
    DESTROYING,
    DESTROYED
} poolState;

typedef void taskFunc (void *);

typedef struct{
    taskFunc* task;
    void* parameter;
}Task;

typedef struct thread_pool
{
    OSQueue* taskQueue; // taskFunc queue
    Task* taskDestroyer; // Task that kill a thread.
    pthread_t* threads;
    pthread_mutex_t queueLock;
    pthread_cond_t queueCondition;
    size_t pool_size;
    poolState state;
}ThreadPool;

ThreadPool* tpCreate(int numOfThreads);

void tpDestroy(ThreadPool* threadPool, int shouldWaitForTasks);

int tpInsertTask(ThreadPool* threadPool, void (*computeFunc) (void *), void* param);
#endif
