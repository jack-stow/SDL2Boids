#include "stats.h"

void Stats_Init(Stats* stats)
{
    memset(stats, 0, sizeof(*stats));

    stats->metrics[STAT_FPS] =
        (StatMetric){ "FPS", "FPS", DBL_MAX, 0.0, 0.0, 0, true };

    stats->metrics[STAT_GRID_MEMSET] =
        (StatMetric){ "Grid Memset", "Memset", DBL_MAX, 0.0, 0.0, 0, true };

    stats->metrics[STAT_GRID_COUNT] =
        (StatMetric){ "Count Grid", "Count", DBL_MAX, 0.0, 0.0, 0, true };

    stats->metrics[STAT_GRID_REDUCE] =
        (StatMetric){ "Grid Reduce", "Reduce", DBL_MAX, 0.0, 0.0, 0, true };

    stats->metrics[STAT_GRID_PREFIX] =
        (StatMetric){ "Grid Prefix", "Prefix", DBL_MAX, 0.0, 0.0, 0, true };

    stats->metrics[STAT_GRID_PREPARE] =
        (StatMetric){ "Grid Prepare", "Prepare", DBL_MAX, 0.0, 0.0, 0, true };

    stats->metrics[STAT_GRID_BUILD] =
        (StatMetric){ "Build Grid", "Build", DBL_MAX, 0.0, 0.0, 0, true };

    stats->metrics[STAT_UPDATE] =
        (StatMetric){ "Update", "Update", DBL_MAX, 0.0, 0.0, 0, true };

    stats->metrics[STAT_DRAW] =
        (StatMetric){ "Draw", "Draw", DBL_MAX, 0.0, 0.0, 0, true };

    stats->metrics[STAT_FRAME_WORK] =
        (StatMetric){ "Frame Work", "Work", DBL_MAX, 0.0, 0.0, 0, true };
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