#pragma once
#include <stdbool.h>
#include <SDL2/SDL_stdinc.h>
#include <float.h>
#include <SDL2/SDL_video.h>

#define STAT_LIST(X) \
    X(FPS,          "FPS",          "FPS",     true) \
    X(GRID_MEMSET,  "Grid Memset",  "Memset",  true) \
    X(GRID_COUNT,   "Count Grid",   "Count",   true) \
    X(GRID_REDUCE,  "Grid Reduce",  "Reduce",  true) \
    X(GRID_PREFIX,  "Grid Prefix",  "Prefix",  true) \
    X(GRID_PREPARE, "Grid Prepare", "Prepare", true) \
    X(GRID_BUILD,   "Build Grid",   "Build",   true) \
    X(FLOCK,        "Flock",        "Flock",   true) \
    X(UPDATE,       "Update",       "Update",  true) \
    X(DRAW,         "Draw",         "Draw",    true) \
    X(FRAME_WORK,   "Frame Work",   "Work",    true)

typedef enum
{
    #define X(id, name, title, show) STAT_##id,
        STAT_LIST(X)
    #undef X
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

typedef struct
{
    Uint64 start[STAT_COUNT];
    bool active[STAT_COUNT];
} FrameProfiler;

void Stats_Init(Stats* stats);

void Stats_AddSample(Stats* stats, StatId id, double value);

double TicksToMs(Uint64 start, Uint64 end, Uint64 freq);

void Stats_ResetSamples(Stats* stats);
double Stats_GetAvg(const Stats* stats, StatId id);

void Profiler_Begin(FrameProfiler* p, StatId id);
void Profiler_End(FrameProfiler* p, Stats* lifetime, Stats* title, StatId id);

void Stats_UpdateTitle(Stats* titleStats, SDL_Window* window, double deltaTime);