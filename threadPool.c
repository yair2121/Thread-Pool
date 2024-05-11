//Yair Yariv Yardeni
#include "threadPool.h"

void runTask(Task* task){
    task->task(task->parameter);
    free(task);
}

Task* createTask(taskFunc computeFunc, void* parameter){
    Task *newTask = malloc(sizeof(Task));
    if(newTask == NULL){
        perror(SYSTEM_CALL_ERROR);
        exit(-1);
    }
    newTask->task = computeFunc;
    newTask->parameter = parameter;
    return newTask;
}

/*
 * This function is used as a Task for a thread to destroy itself.
*/
void ThreadSuicideTask(void *args) {
    ThreadPool *threadPool = (ThreadPool *) args;
    pthread_cond_signal(&threadPool->queueCondition); //Signal the next thread to start his suicide.
    pthread_exit(args);
}

/*
 * This function is used as a task for a thread to execute tasks from its owner thread pool.
 * The 'args' parameter represents the owner thread pool.
 */
_Noreturn void startThread(void *args) {
    ThreadPool *threadPool = (ThreadPool *) args;
    Task *task;
      while (TRUE) {
          //Start Critical Section.
          pthread_mutex_lock(&threadPool->queueLock);
          while (osIsQueueEmpty(threadPool->taskQueue)) { // Wait for task to be added to queue.
              pthread_cond_wait(&threadPool->queueCondition, &threadPool->queueLock);
          }
          task = (Task *) osDequeue(threadPool->taskQueue);
          pthread_mutex_unlock(&threadPool->queueLock);
          //End Critical Section
          runTask(task);
        }
    }

/*
 * Allocate all needed memory on heap for threadPool.
 */
ThreadPool *allocateThreadPool(int numOfThreads) {
    ThreadPool *threadPool;
    threadPool = malloc(sizeof(ThreadPool));
    if(threadPool == NULL){
        perror(SYSTEM_CALL_ERROR);
        exit(-1);
    }
    threadPool->threads = malloc(sizeof(pthread_t) * numOfThreads);
    threadPool->taskDestroyer = createTask(ThreadSuicideTask, threadPool);
    if(threadPool->taskDestroyer == NULL || threadPool->threads == NULL){
        perror(SYSTEM_CALL_ERROR);
        exit(-1);
    }
    threadPool->taskQueue = osCreateQueue();
    return threadPool;
}

void initThreadPoolLocks(ThreadPool* threadPool){
    pthread_mutex_init(&threadPool->queueLock, NULL);
    pthread_cond_init(&threadPool->queueCondition, NULL);
}

void initThreadPoolThreads(ThreadPool* threadPool){
    int threadIndex;
    for (threadIndex = 0; threadIndex < threadPool->pool_size; ++threadIndex) {
        pthread_create(&threadPool->threads[threadIndex], NULL, (void*) startThread, threadPool);
    }
}

void freeThreadPoolMemory(ThreadPool* threadPool){
    osDestroyQueue(threadPool->taskQueue);
    free(threadPool->threads);
    free(threadPool->taskDestroyer);
    pthread_mutex_destroy(&threadPool->queueLock);
    pthread_cond_destroy(&threadPool->queueCondition);
    free(threadPool);
}

/*
 * Create a new thread pool on the heap.
*/
ThreadPool *tpCreate(int numOfThreads) {
    ThreadPool *threadPool = allocateThreadPool(numOfThreads);
    threadPool->state = CREATING;
    threadPool->pool_size = numOfThreads;
    initThreadPoolLocks(threadPool);
    initThreadPoolThreads(threadPool);
    threadPool->state = RUNNING;
    return threadPool;
}

/*
 * This function causes the current thread to wait for the threads in the specified thread pool to finish their tasks.
 */
void joinThreads(ThreadPool* threadPool){
    int threadIndex;
    for(threadIndex = 0; threadIndex < threadPool->pool_size; ++threadIndex){
        pthread_join(threadPool->threads[threadIndex], NULL);
    }
}

void emptyQueue(OSQueue* queue){
    while (osIsQueueEmpty(queue) == FALSE){
        free(osDequeue(queue));
    }
}

/*
 * Insert to the thread pool the "self-destroy" task for each thread in the pool.
*/
void destroyThreads(ThreadPool* threadPool){
    int poolIndex;
    for(poolIndex = 0; poolIndex < threadPool->pool_size; ++poolIndex){
        osEnqueue(threadPool->taskQueue, threadPool->taskDestroyer);
    }
}

void tpDestroy(ThreadPool *threadPool, int shouldWaitForTasks) {
    if (threadPool->state < DESTROYING) {
        threadPool->state = DESTROYING;

        //Start Critical Section
        pthread_mutex_lock(&threadPool->queueLock);
        if(!shouldWaitForTasks) {
            emptyQueue(threadPool->taskQueue);
        }
        destroyThreads(threadPool);
        pthread_mutex_unlock(&threadPool->queueLock);
        //End Critical Section

        pthread_cond_signal(&threadPool->queueCondition); // Start thread pool self destroying sequence.
        while (osIsQueueEmpty(threadPool->taskQueue) == FALSE){
            pthread_cond_wait(&threadPool->queueCondition, &threadPool->queueLock);
        }
        joinThreads(threadPool);
        threadPool->state = DESTROYED;
        freeThreadPoolMemory(threadPool);
    }
}

int tpInsertTask(ThreadPool *threadPool, void (*computeFunc)(void *), void *param) {
    if (threadPool->state >= DESTROYING) {
        return UNSUCCESSFUL;
    }
    Task *newTask = createTask(computeFunc, param);

    //Start Critical Section
    pthread_mutex_lock(&threadPool->queueLock);
    osEnqueue(threadPool->taskQueue, newTask);
    pthread_mutex_unlock(&threadPool->queueLock);
    //End Critical Section

    pthread_cond_signal(&threadPool->queueCondition);
    return SUCCESSFUL;
}
