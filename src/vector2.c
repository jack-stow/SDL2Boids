/* Quackmatic 2014
 * https://github.com/Quackmatic
 */

#include <math.h>
#include "vector2.h"

vec2 vec2_zero(vec2* v)
{
    v->x = 0;
    v->y = 0;
    return *v;
}

vec2 vec_add(vec2 v1, vec2 v2)
{
    vec2 value = { v1.x + v2.x, v1.y + v2.y };
    return value;
}

vec2 vec_sub(vec2 v1, vec2 v2)
{
    vec2 value = { v1.x - v2.x, v1.y - v2.y };
    return value;
}

vec2 vec_mul(vec2 v, double d)
{
    vec2 value = { v.x * d, v.y * d };
    return value;
}

double vec_mag(vec2 v)
{
    return sqrt(v.x * v.x + v.y * v.y);
}

vec2 vec_norm(vec2 v)
{
    double mag = vec_mag(v);

    if (mag == 0.0) {
        v = (vec2){ 0, 0 };
        return v;
    }

    return vec_mul(v, 1.0 / mag);
}

double vec_dot(vec2 v1, vec2 v2)
{
    return v1.x * v2.x + v1.y * v2.y;
}

double vec_dist(vec2 v1, vec2 v2)
{
    return vec_mag(vec_sub(v1, v2));
}

double vec_angle(vec2 v1, vec2 v2)
{
    double dot = vec_dot(v1, v2);
    double mag1 = vec_mag(v1);
    double mag2 = vec_mag(v2);

    if (mag1 == 0.0 || mag2 == 0.0)
    {
        return 0.0;
    }

    double cosine = dot / (mag1 * mag2);

    // Clamp to avoid floating point precision issues
    if (cosine > 1.0)
    {
        cosine = 1.0;
    }
    else if (cosine < -1.0)
    {
        cosine = -1.0;
    }

    return acos(cosine);
}

vec2 vec_lerp(vec2 a, vec2 b, double t)
{
    vec2 result;

    result.x = a.x + (b.x - a.x) * t;
    result.y = a.y + (b.y - a.y) * t;

    return result;
}