//
//#include "Vec2.h"
//
//Vec2 vec2(float x, float y)
//{
//	Vec2 v;
//	v.x = x;
//	v.y = y;
//	return v;
//}
//
//Vec2 vec2_init_zero()
//{
//	return vec2(0, 0);
//}
//
//Vec2 vec2_init_one()
//{
//	return vec2(1, 1);
//}
//
//void vec2_zero(Vec2* v)
//{
//	v->x = 0;
//	v->y = 0;
//}
//
//Vec2 vec2_add(Vec2 a, Vec2 b)
//{
//	return vec2(a.x + b.x, a.y + b.y);
//}
//
//Vec2 vec2_sub(Vec2 a, Vec2 b)
//{
//	return vec2(a.x - b.x, a.y - b.y);
//}
//
//Vec2 vec2_mul(Vec2 v, float s)
//{
//	return vec2(v.x * s, v.y * s);
//}
//
//Vec2 vec2_div(Vec2 v, float s)
//{
//	return vec2(v.x / s, v.y / s);
//}
//
//Vec2 vec2_negate(Vec2 v)
//{
//	return vec2(-v.x, -v.y);
//}
//
//float vec2_length(Vec2 v)
//{
//	return sqrt(v.x * v.x + v.y * v.y);
//}
//
//float vec2_length_squared(Vec2 v)
//{
//	return v.x * v.x + v.y * v.y;
//}
//
//Vec2 vec2_normalize(Vec2 v)
//{
//	float len = vec2_length(v);
//	if (len == 0) {
//		return vec2_init_zero();
//	}
//	return vec2_div(v, len);
//}
//
//float vec2_distance(Vec2 a, Vec2 b)
//{
//	return vec2_length(vec2_sub(a, b));
//}
//
//float vec2_distance_squared(Vec2 a, Vec2 b)
//{
//	return vec2_length_squared(vec2_sub(a, b));
//}
//
//float vec2_dot(Vec2 a, Vec2 b)
//{
//	return a.x * b.x + a.y * b.y;
//}
//
//float vec2_cross(Vec2 a, Vec2 b)
//{
//	return a.x * b.y - a.y * b.x;
//}
//
//Vec2 vec2_lerp(Vec2 a, Vec2 b, float t)
//{
//	return vec2_add(vec2_mul(a, 1 - t), vec2_mul(b, t));
//}
//
//Vec2 vec2_clamp_magnitude(Vec2 v, float maxLen)
//{
//	float len = vec2_length(v);
//	if (len > maxLen) {
//		return vec2_mul(vec2_div(v, len), maxLen);
//	}
//	return v;
//}
//
//Vec2 vec2_reflect(Vec2 v, Vec2 normal)
//{
//	float dot = vec2_dot(v, normal);
//	return vec2_sub(v, vec2_mul(normal, 2 * dot));
//}
//
//float vec2_angle(Vec2 v)
//{
//	return atan2(v.y, v.x);
//}
//
//float vec2_angle_between(Vec2 a, Vec2 b)
//{
//	float dot = vec2_dot(a, b);
//	float lenA = vec2_length(a);
//	float lenB = vec2_length(b);
//	if (lenA == 0 || lenB == 0) {
//		return 0;
//	}
//	return acos(dot / (lenA * lenB));
//}
//
//Vec2 vec2_rotate(Vec2 v, float radians)
//{
//	float cosTheta = cos(radians);
//	float sinTheta = sin(radians);
//	return vec2(v.x * cosTheta - v.y * sinTheta, v.x * sinTheta + v.y * cosTheta);
//}
//
//Vec2 vec2_limit(Vec2 v, float max)
//{
//	float len = vec2_length(v);
//	if (len > max) {
//		return vec2_mul(vec2_div(v, len), max);
//	}
//	return v;
//}
