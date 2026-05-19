#include "common.h"

double normalize_angle(double a)
{
    while (a > 180.0) a -= 360.0;
    while (a < -180.0) a += 360.0;
    return a;
}

double angle_lerp(double a, double b, double t)
{
    double diff = normalize_angle(b - a);
    return a + diff * t;
}


int rand_range(int min, int max)
{
    return min + rand() % (max - min + 1);
}

double rand_range_double(double min, double max)
{
    double t = (double)rand() / (double)RAND_MAX;
    return min + t * (max - min);
}