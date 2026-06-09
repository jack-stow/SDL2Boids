#include "workerpool.h"

void WorkerPool_Run(WorkerPool* pool, int itemCount, int chunkSize, int activeThreads, WorkerJobFunc jobFunc, void* jobData)
{
    if (activeThreads > pool->numThreads)
        activeThreads = pool->numThreads;

    if (activeThreads < 1)
        activeThreads = 1;


    SDL_LockMutex(pool->mutex);

    pool->nextChunkStart = 0;
    pool->chunkSize = chunkSize;
    pool->itemCount = itemCount;
    pool->activeThreads = activeThreads;
    pool->jobFunc = jobFunc;
    pool->jobData = jobData;

    pool->completed = 0;
    pool->generation++;

    SDL_CondBroadcast(pool->startCond);

    while (pool->completed < pool->numThreads)
    {
        SDL_CondWait(pool->doneCond, pool->mutex);
    }

    SDL_UnlockMutex(pool->mutex);
}



int PersistentWorkerMainBalanced(void* data)
{
    int threadIndex = *(int*)data;
    int lastGeneration = 0;

    while (1)
    {
        SDL_LockMutex(workerpool.mutex);

        while (!workerpool.quit && workerpool.generation == lastGeneration)
        {
            SDL_CondWait(workerpool.startCond, workerpool.mutex);
        }

        if (workerpool.quit)
        {
            SDL_UnlockMutex(workerpool.mutex);
            return 0;
        }

        lastGeneration = workerpool.generation;

        if (threadIndex >= workerpool.activeThreads)
        {
            workerpool.completed++;

            if (workerpool.completed == workerpool.numThreads)
            {
                SDL_CondSignal(workerpool.doneCond);
            }

            SDL_UnlockMutex(workerpool.mutex);
            continue;
        }

        WorkerJobFunc jobFunc = workerpool.jobFunc;
        void* jobData = workerpool.jobData;

        SDL_UnlockMutex(workerpool.mutex);

        while (1)
        {
            SDL_LockMutex(workerpool.chunkMutex);

            int start = workerpool.nextChunkStart;
            workerpool.nextChunkStart += workerpool.chunkSize;

            SDL_UnlockMutex(workerpool.chunkMutex);

            if (start >= workerpool.itemCount)
            {
                break;
            }

            int end = start + workerpool.chunkSize;

            if (end > workerpool.itemCount)
            {
                end = workerpool.itemCount;
            }

            jobFunc(jobData, start, end, threadIndex);
        }

        SDL_LockMutex(workerpool.mutex);

        workerpool.completed++;

        if (workerpool.completed == workerpool.numThreads)
        {
            SDL_CondSignal(workerpool.doneCond);
        }

        SDL_UnlockMutex(workerpool.mutex);
    }
}