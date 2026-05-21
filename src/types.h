#pragma once
#include <float.h>


//#define USE_DOUBLE

#ifdef USE_DOUBLE
    typedef double real;
    #define REAL_MAX DBL_MAX
    #define REAL_TYPE_NAME "double"
#else
    typedef float real;
    #define REAL_MAX FLT_MAX
    #define REAL_TYPE_NAME "float"
#endif