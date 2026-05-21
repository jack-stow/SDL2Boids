#pragma once
#define SDL_MAIN_HANDLED
#include "ctype.h"
#include "defs.h"
#include "math.h"
#include "SDL2/SDL.h"
#include "stdio.h"
#include "stdlib.h"
#include <stdbool.h>
#include "string.h"
#include "structs.h"
#include "vector2.h"
#include "draw.h"
#include <float.h>


#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#define CLAMP(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))
#define LERP(a, b, t) ((a) + (t) * ((b) - (a)))

double angle_lerp(double a, double b, double t);
double normalize_angle(double a);
double rand_range_double(double min, double max);

typedef struct
{
    double runTime;

    double minFps;
    double maxFps;
    double fpsSum;
    int fpsSamples;

    double minUpdateMs;
    double maxUpdateMs;
    double updateMsSum;
    int updateSamples;

    double minDrawMs;
    double maxDrawMs;
    double drawMsSum;
    int drawSamples;

    int totalFrames;

    double minFrameWorkMs;
    double maxFrameWorkMs;
    double frameWorkMsSum;
    int frameWorkSamples;

} Stats;

extern Stats stats;