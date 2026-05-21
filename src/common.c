#include "common.h"

real normalize_angle(real a)
{
    while (a > HALF_CIRCLE_DEGREES)
        a -= FULL_CIRCLE_DEGREES;

    while (a < -HALF_CIRCLE_DEGREES)
        a += FULL_CIRCLE_DEGREES;

    return a;
}
real angle_lerp(real a, real b, real t)
{
    real diff = normalize_angle(b - a);
    return a + diff * t;
}


int rand_range(int min, int max)
{
    return min + rand() % (max - min + 1);
}

real rand_range_real(real min, real max)
{
    real t = R(rand()) / R(RAND_MAX);
    return min + t * (max - min);
}