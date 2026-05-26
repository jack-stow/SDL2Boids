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