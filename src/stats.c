#include "stats.h"

void Stats_Init(Stats* stats)
{
    memset(stats, 0, sizeof(*stats));

    #define X(id, displayName, titleName, show) \
        stats->metrics[STAT_##id] = \
            (StatMetric){ displayName, titleName, DBL_MAX, 0.0, 0.0, 0, show };

        STAT_LIST(X)

    #undef X
}

void Stats_AddSample(Stats* stats, StatId id, double value)
{
    StatMetric* m = &stats->metrics[id];

    if (value < m->min) m->min = value;
    if (value > m->max) m->max = value;

    m->sum += value;
    m->samples++;
}

double TicksToMs(Uint64 start, Uint64 end, Uint64 freq)
{
    return (double)(end - start) / (double)freq * 1000.0;
}

void Stats_ResetSamples(Stats* stats)
{
    for (int i = 0; i < STAT_COUNT; i++)
    {
        StatMetric* m = &stats->metrics[i];

        m->min = DBL_MAX;
        m->max = 0.0;
        m->sum = 0.0;
        m->samples = 0;
    }

    stats->runTime = 0.0;
    stats->totalFrames = 0;
}

double Stats_GetAvg(const Stats* stats, StatId id)
{
    const StatMetric* m = &stats->metrics[id];

    if (m->samples == 0)
    {
        return 0.0;
    }

    return m->sum / m->samples;
}



void Profiler_Begin(FrameProfiler* p, StatId id)
{
    p->start[id] = SDL_GetPerformanceCounter();
    p->active[id] = true;
}

void Profiler_End(FrameProfiler* p, Stats* lifetime, Stats* title, StatId id)
{
    if (!p->active[id])
    {
        return;
    }

    Uint64 end = SDL_GetPerformanceCounter();
    double ms = TicksToMs(p->start[id], end, SDL_GetPerformanceFrequency());

    Stats_AddSample(lifetime, id, ms);

    if (title != NULL)
    {
        Stats_AddSample(title, id, ms);
    }

    p->active[id] = false;
}

void Stats_UpdateTitle(Stats* titleStats, SDL_Window* window, double deltaTime)
{
    static double titleTimer = 0.0;

    titleTimer += deltaTime;

    if (titleTimer < 1.0)
    {
        return;
    }


    char title[1024] = { 0 };
    size_t offset = 0;

    for (int i = 0; i < STAT_COUNT; i++)
    {
        StatMetric* metric = &titleStats->metrics[i];

        if (!metric->showInTitle)
        {
            continue;
        }

        if (metric->samples == 0)
            continue;

        offset += snprintf(
            title + offset,
            sizeof(title) - offset,
            "%s: %.3f | ",
            metric->titleName,
            Stats_GetAvg(titleStats, (StatId)i));
    }

    SDL_SetWindowTitle(window, title);

    Stats_ResetSamples(titleStats);
    titleTimer = 0.0;
}