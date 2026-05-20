#include "common.h"

#ifndef BOID_H
#define BOID_H

#define BOID_SCALE 0.1
#define FRAME_RATE 60.0

#define TOP_SPEED        (5.0 * FRAME_RATE)
#define MIN_SPEED        (TOP_SPEED * 0.4)
#define TURN_SPEED       (30.0 / TOP_SPEED)
#define ACCELERATION     (0.1 * FRAME_RATE)

#define AVOID_FACTOR     (1.35 * FRAME_RATE)
#define MATCHING_FACTOR  (0.1 * FRAME_RATE)
#define CENTERING_FACTOR (0.1 * FRAME_RATE)
#define BORDERING_FACTOR (5.0 * FRAME_RATE)
#define POI_FACTOR       (10.0)


#define MAX_VISIBLE        10.0
#define VISION_RADIUS      70.0
#define PROTECTED_RANGE    5.0


#define BOID_COUNT 	  500



typedef struct
{
	double          x;
	double          y;

	vec2 speed;
	vec2 desiredSpeed;

	double angle;
	double desiredAngle;


	SDL_Texture* texture;
} Boid;

typedef struct
{
	double topSpeed;
	double minSpeed;
	double turnSpeed;
	double acceleration;

	double avoidFactor;
	double matchingFactor;
	double centeringFactor;
	double borderingFactor;

	int maxVisible;
	double visionRadius;
	double visionRadiusSq;
	double protectedRange;
	double protectedRangeSq;
	double poiFactor;

} SimulationParameters;

Boid boid_create(SimulationParameters sim, char* texture);


void drawBoid(Boid* boid);


#endif