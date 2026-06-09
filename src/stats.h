#pragma once
#include <stdbool.h>
#include <SDL2/SDL_stdinc.h>
#include <float.h>

typedef enum
{
    STAT_FPS,
    STAT_GRID_MEMSET,
    STAT_GRID_COUNT,
    STAT_GRID_REDUCE,
    STAT_GRID_PREFIX,
    STAT_GRID_PREPARE,
    STAT_GRID_BUILD,
    STAT_UPDATE,
    STAT_DRAW,
    STAT_FRAME_WORK,
    STAT_COUNT
} StatId;

typedef struct
{
    const char* name;
    const char* titleName;
    double min;
    double max;
    double sum;
    int samples;
    bool showInTitle;
} StatMetric;

typedef struct
{
    double runTime;
    int totalFrames;
    StatMetric metrics[STAT_COUNT];
} Stats;

void Stats_Init(Stats* stats);

void Stats_AddSample(Stats* stats, StatId id, double value);

double TicksToMs(Uint64 start, Uint64 end, Uint64 freq);