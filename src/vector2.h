#pragma once
#include "types.h"

typedef struct { real x; real y; } vec2;

vec2 vec_add(vec2 v1, vec2 v2);

vec2 vec_sub(vec2 v1, vec2 v2);

vec2 vec_mul(vec2 v, real d);

real vec_mag(vec2 v);
real vec_mag_sq(vec2 v);

vec2 vec_norm(vec2 v);

real vec_dot(vec2 v1, vec2 v2);

real vec_dist(vec2 v1, vec2 v2);

real vec_dist_sq(vec2 v1, vec2 v2);

real vec_angle(vec2 v1, vec2 v2);

vec2 vec2_zero(vec2* v);

vec2 vec_lerp(vec2 a, vec2 b, real t);

vec2 vec_clamp_mag(vec2 v, real minSpeed, real maxSpeed);
