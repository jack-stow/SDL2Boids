//typedef struct
//{
//    int          x;
//    int          y;
//} Vec2;
//
//
//Vec2 vec2(float x, float y);
//Vec2 vec2_init_zero();
//Vec2 vec2_init_one(); 
//void vec2_zero(Vec2* v);
//
//#pragma once
//Vec2 vec2_add(Vec2 a, Vec2 b);
//Vec2 vec2_sub(Vec2 a, Vec2 b);
//Vec2 vec2_mul(Vec2 v, float s);
//Vec2 vec2_div(Vec2 v, float s);
//Vec2 vec2_negate(Vec2 v);
//
//
//float vec2_length(Vec2 v);
//float vec2_length_squared(Vec2 v);
//Vec2 vec2_normalize(Vec2 v);
//
//
//float vec2_distance(Vec2 a, Vec2 b);
//float vec2_distance_squared(Vec2 a, Vec2 b);
//float vec2_dot(Vec2 a, Vec2 b);
//
//
//float vec2_cross(Vec2 a, Vec2 b);
//
//Vec2 vec2_lerp(Vec2 a, Vec2 b, float t);
//
//Vec2 vec2_clamp_magnitude(Vec2 v, float maxLen);
//Vec2 vec2_reflect(Vec2 v, Vec2 normal);
//
//float vec2_angle(Vec2 v);
//float vec2_angle_between(Vec2 a, Vec2 b);
//
//Vec2 vec2_rotate(Vec2 v, float radians);
//
//Vec2 vec2_limit(Vec2 v, float max);