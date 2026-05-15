/*
 * Copyright (C) 2015-2018,2022 Parallel Realities. All rights reserved.
 */
#define SDL_MAIN_HANDLED
#include "ctype.h"
#include "defs.h"
#include "math.h"
#include "SDL2/SDL.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "structs.h"
#include "vector2.h"
#include "draw.h"


#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#define CLAMP(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))
#define LERP(a, b, t) ((a) + (t) * ((b) - (a)))

double angle_lerp(double a, double b, double t);
double normalize_angle(double a);