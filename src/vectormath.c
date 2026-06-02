#include "vectormath.h"

real vec_mag_xy(real x, real y)
{
    return REAL_SQRT(x * x + y * y);
}

real vec_mag_sq_xy(real x, real y)
{
    return x * x + y * y;
}

real vec_dist_sq_xy(real x1, real y1, real x2, real y2)
{
    real dx = x1 - x2;
    real dy = y1 - y2;

    return dx * dx + dy * dy;
}

real vec_dist_xy(real x1, real y1, real x2, real y2)
{
    return REAL_SQRT(
        vec_dist_sq_xy(x1, y1, x2, y2)
    );
}

real vec_dot_xy(real x1, real y1, real x2, real y2)
{
    return x1 * x2 + y1 * y2;
}

void vec_norm_xy(real x, real y, real* outX, real* outY)
{
    real mag = vec_mag_xy(x, y);

    if (mag == R(0.0))
    {
        *outX = R(0.0);
        *outY = R(0.0);
        return;
    }

    real invMag = R(1.0) / mag;

    *outX = x * invMag;
    *outY = y * invMag;
}

void vec_clamp_mag_xy(
    real x,
    real y,
    real minMag,
    real maxMag,
    real* outX,
    real* outY
)
{
    real mag = vec_mag_xy(x, y);

    if (mag == R(0.0))
    {
        *outX = x;
        *outY = y;
        return;
    }

    real targetMag = mag;

    if (mag > maxMag)
        targetMag = maxMag;
    else if (mag < minMag)
        targetMag = minMag;

    real scale = targetMag / mag;

    *outX = x * scale;
    *outY = y * scale;
}

static real cross(vec2 a, vec2 b)
{
    return a.x * b.y - a.y * b.x;
}

bool line_segments_intersect(
    vec2 p1,
    vec2 p2,
    vec2 q1,
    vec2 q2)
{
    vec2 r = vec_sub(p2, p1);
    vec2 s = vec_sub(q2, q1);

    real rxs = cross(r, s);
    real qpxr = cross(vec_sub(q1, p1), r);

    // Parallel
    if (REAL_FABS(rxs) < R(0.0001))
    {
        // Collinear
        if (REAL_FABS(qpxr) < R(0.0001))
        {
            real rr = vec_dot(r, r);

            if (rr < R(0.0001))
                return false;

            real t0 = vec_dot(vec_sub(q1, p1), r) / rr;
            real t1 = vec_dot(vec_sub(q2, p1), r) / rr;

            if (t0 > t1)
            {
                real tmp = t0;
                t0 = t1;
                t1 = tmp;
            }

            return t0 <= R(1.0) && t1 >= R(0.0);
        }

        return false;
    }

    real t = cross(vec_sub(q1, p1), s) / rxs;
    real u = cross(vec_sub(q1, p1), r) / rxs;

    return
        t >= R(0.0) &&
        t <= R(1.0) &&
        u >= R(0.0) &&
        u <= R(1.0);
}