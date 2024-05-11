## Thread Pool Implementation in C
## Overview
This thread pool implementation provides a simple and efficient way to manage a pool of worker threads for executing tasks concurrently.

## Features
Dynamic thread creation and management
Task queue for scheduling work
Thread-safe synchronization using mutex and condition variables
Graceful shutdown with optional waiting for pending tasks
## Getting Started
Include the Header File:
```
#include "thread_pool.h"
```
## Create a Thread Pool:
```
ThreadPool* tp = tpCreate(numOfThreads);
if (!tp) {
    fprintf(stderr, "Error creating thread pool\n");
    exit(EXIT_FAILURE);
}
```
## Insert Tasks:
```
void myTask(void* param) {
    // Your task logic here
}

// Insert a task into the thread pool
tpInsertTask(tp, myTask, someParam);
```
## Destroy the Thread Pool:
```
// Clean up resources
tpDestroy(tp, shouldWaitForTasks);
```

