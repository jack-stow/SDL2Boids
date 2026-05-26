#pragma once

#include "types.h"

real vec_mag_xy(real x, real y);

real vec_mag_sq_xy(real x, real y);

real vec_dist_sq_xy(real x1, real y1, real x2, real y2);

real vec_dist_xy(real x1, real y1, real x2, real y2);

real vec_dot_xy(real x1, real y1, real x2, real y2);

void vec_norm_xy(real x, real y, real* outX, real* outY);

void vec_clamp_mag_xy(
    real x,
    real y,
    real minMag,
    real maxMag,
    real* outX,
    real* outY
);

void vec_lerp_xy(
    real ax,
    real ay,
    real bx,
    real by,
    real t,
    real* outX,
    real* outY
);