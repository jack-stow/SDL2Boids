#pragma once
#include <SDL2/SDL_mutex.h>
#include <SDL2/SDL_thread.h>

typedef void (*WorkerJobFunc)(void* jobData, int start, int end, int threadIndex);

typedef struct
{
    SDL_mutex* mutex;
    SDL_cond* startCond;
    SDL_cond* doneCond;
    SDL_mutex* chunkMutex;

    int quit;
    int generation;
    int completed;
    int numThreads;

    int nextChunkStart;
    int chunkSize;
    int itemCount;

    WorkerJobFunc jobFunc;
    void* jobData;
} WorkerPool;

WorkerPool workerpool;

int PersistentWorkerMainBalanced(void* data);

void WorkerPool_Run(
    WorkerPool* pool,
    int itemCount,
    int chunkSize,
    WorkerJobFunc jobFunc,
    void* jobData
);
