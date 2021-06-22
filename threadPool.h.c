//Yair Yariv Yardeni 315009969
#include "threadPool.h"
#include "stdlib.h"


ThreadPool* tpCreate(int numOfThreads){
    ThreadPool* threadPool = malloc(sizeof (ThreadPool));
    threadPool->pool_size= numOfThreads;
    int threadCreated = 0;
    for(; threadCreated < numOfThreads; ++threadCreated){
        //TODO: create and insert new thread to threadPool;
    }
}

void tpDestroy(ThreadPool* threadPool, int shouldWaitForTasks);

int tpInsertTask(ThreadPool* threadPool, void (*computeFunc) (void *), void* param);