
#ifndef VECTOR2_H
#define VECTOR2_H

typedef struct { double x; double y; } vec2;

vec2 vec_add(vec2 v1, vec2 v2);

vec2 vec_sub(vec2 v1, vec2 v2);

vec2 vec_mul(vec2 v, double d);

double vec_mag(vec2 v);
double vec_mag_sq(vec2 v);

vec2 vec_norm(vec2 v);

double vec_dot(vec2 v1, vec2 v2);

double vec_dist(vec2 v1, vec2 v2);

double vec_dist_sq(vec2 v1, vec2 v2);

double vec_angle(vec2 v1, vec2 v2);

vec2 vec2_zero(vec2* v);

vec2 vec_lerp(vec2 a, vec2 b, double t);

vec2 vec_clamp_mag(vec2 v, double minSpeed, double maxSpeed);

#endif