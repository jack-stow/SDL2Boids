#include "common.h"

#ifndef BOID_H
#define BOID_H

#define BOID_SCALE 0.2

#define TOP_SPEED         5.0
#define MIN_SPEED         (TOP_SPEED * 0.4)
#define ACCELERATION      0.08

#define AVOID_FACTOR      0.25
#define MATCHING_FACTOR   0.2
#define CENTERING_FACTOR  0.1
#define BORDERING_FACTOR  5.0
#define POI_FACTOR         1.0


#define MAX_VISIBLE        20
#define VISION_RADIUS      70.0
#define PROTECTED_RANGE    30.0


#define BOID_COUNT 	  800



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