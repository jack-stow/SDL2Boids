#pragma once
#include "types.h"
#include <stdbool.h>
#include "camera.h"
typedef struct Obstacles
{
	real x1, y1, x2, y2;
	struct Obstacles* next;
} Obstacles;

void Obstacles_Add(Obstacles** obstacles, real x1, real y1, real x2, real y2);
void Obstacles_Free(Obstacles* obstacles);
void Obstacles_RemoveOne(Obstacles* obstacles, Obstacles* toRemove);
void Obstacles_Draw(Camera* camera, Obstacles* obstacles);
int Obstacles_Count(const Obstacles* obstacles);
bool Obstacle_IntersectsLine(const Obstacles* obstacles, real x1, real y1, real x2, real y2);
void Obstacles_EraseIntersecting(Obstacles** obstacles, real x1, real y1, real x2, real y2);