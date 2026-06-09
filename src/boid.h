#include "common.h"
#include "camera.h"

#ifndef BOID_H
#define BOID_H

#define BOID_SCALE        (R(0.05))

// Real casting macro kept to the outside because FRAME_RATE is a double.
#define TOP_SPEED         (R(4.0 * FRAME_RATE))
#define MIN_SPEED         (TOP_SPEED * R(0.4))
#define TURN_SPEED        (R(30.0) / TOP_SPEED)
#define ACCELERATION      (R(0.1 * FRAME_RATE))

#define AVOID_FACTOR      (R(1.35 * FRAME_RATE))
#define MATCHING_FACTOR   (R(0.2 * FRAME_RATE))
#define CENTERING_FACTOR  (R(0.2 * FRAME_RATE))
#define BORDERING_FACTOR  (R(5.0 * FRAME_RATE))
#define OBSTACLE_AVOID_FACTOR (R(50.0 * FRAME_RATE))

#define POI_FACTOR        (R(10.0))

#define OBSTACLE_AVOID_DISTANCE (R(10.0))

#define BOID_COUNT 	      (1000000)

#define MAX_VISIBLE       (4)
#define VISION_RADIUS     (R(0.9))
#define PROTECTED_RANGE   (R(0.25))


#define BOID_TEXTURE      "gfx/boid.png"

typedef struct
{
	real          x;
	real          y;

	vec2 speed;

	real angle;

	
} Boid;

typedef struct
{
	real topSpeed;
	real minSpeed;
	real turnSpeed;
	real acceleration;

	real scale;

	real avoidFactor;
	real matchingFactor;
	real centeringFactor;
	real borderingFactor;
	real obstacleAvoidFactor;

	int maxVisible;
	real visionRadius;
	real visionRadiusSq;
	real protectedRange;
	real protectedRangeSq;
	real poiFactor;

	real obstacleAvoidDistance;

	SDL_Texture* texture;
	DrawColor boidColor;

} SimulationParameters;

Boid boid_create(SimulationParameters* sim);


void drawBoid(Camera* camera, Boid* boid, SimulationParameters* sim);


#endif