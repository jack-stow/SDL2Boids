#include "common.h"

#ifndef BOID_H
#define BOID_H
typedef struct
{
	double          x;
	double          y;

	vec2 speed;
	vec2 desiredSpeed;

	double angle;
	double desiredAngle;

	double minSpeed;
	double topSpeed;
	double acceleration;


	SDL_Texture* texture;
} Boid;



Boid boid_create(double topSpeed, double minSpeed, double acceleration, char* texture);


void drawBoid(Boid* boid);


#endif