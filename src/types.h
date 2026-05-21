#pragma once
#include <float.h>


//#define USE_DOUBLE

#ifdef USE_DOUBLE
    typedef double real;
    #define REAL_MAX DBL_MAX
    #define REAL_SQRT sqrt
    #define REAL_ACOS acos
    #define REAL_ATAN2 atan2
    #define REAL_TYPE_NAME "double"
#else
    typedef float real;
    #define REAL_MAX FLT_MAX
    #define REAL_SQRT sqrtf
    #define REAL_ACOS acosf
    #define REAL_ATAN2 atan2f
    #define REAL_TYPE_NAME "float"
#endif

#define REAL_PI ((real)3.14159265358979323846)

#define R(x) ((real)(x))