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

vec2 vec_mul(vec2 v, real d)
{
    vec2 value = { v.x * d, v.y * d };
    return value;
}

real vec_mag(vec2 v)
{
    return REAL_SQRT(v.x * v.x + v.y * v.y);
}

real vec_mag_sq(vec2 v)
{
    return v.x * v.x + v.y * v.y;
}

/// <summary>
/// produces a unit vector in the same direction as the input vector. 
/// If the input vector has zero magnitude, it returns a zero vector.
/// returns a new vector that is the normalized version of the input vector. (new struct)
/// </summary>
/// <param name="v"></param>
/// <returns></returns>
vec2 vec_norm(vec2 v)
{
    real mag = vec_mag(v);

    if (mag == 0.0) {
        v = (vec2){ 0, 0 };
        return v;
    }

    return vec_mul(v, R(1.0) / mag);
}

real vec_dot(vec2 v1, vec2 v2)
{
    return v1.x * v2.x + v1.y * v2.y;
}

real vec_dist(vec2 v1, vec2 v2)
{
    return vec_mag(vec_sub(v1, v2));
}

real vec_dist_sq(vec2 v1, vec2 v2)
{
    vec2 diff = vec_sub(v1, v2);

    return
        diff.x * diff.x +
        diff.y * diff.y;
}

real vec_angle(vec2 v1, vec2 v2)
{
    real dot = vec_dot(v1, v2);
    real mag1 = vec_mag(v1);
    real mag2 = vec_mag(v2);

    if (mag1 == 0.0 || mag2 == 0.0)
    {
        return 0.0;
    }

    real cosine = dot / (mag1 * mag2);

    // Clamp to avoid floating point precision issues
    if (cosine > 1.0)
    {
        cosine = 1.0;
    }
    else if (cosine < -1.0)
    {
        cosine = -1.0;
    }

    return REAL_ACOS(cosine);
}

vec2 vec_lerp(vec2 a, vec2 b, real t)
{
    vec2 result;

    result.x = a.x + (b.x - a.x) * t;
    result.y = a.y + (b.y - a.y) * t;

    return result;
}

vec2 vec_clamp_mag(vec2 v, real minSpeed, real maxSpeed)
{
    real mag = vec_mag(v);

    if (mag == 0.0)
    {
        return v;
    }

    if (mag > maxSpeed)
    {
        return vec_mul(vec_norm(v), maxSpeed);
    }

    if (mag < minSpeed)
    {
        return vec_mul(vec_norm(v), minSpeed);
    }

    return v;
}