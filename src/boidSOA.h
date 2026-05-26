#include "common.h"

#ifndef BOIDSOA_H
#define BOIDSOA_H

#define BOID_SCALE        (R(0.1))

// Real casting macro kept to the outside because FRAME_RATE is a double.
#define TOP_SPEED_SOA         (R(5.0 * FRAME_RATE))
#define MIN_SPEED_SOA         (TOP_SPEED_SOA * R(0.4))
#define TURN_SPEED_SOA        (R(30.0) / TOP_SPEED_SOA)
#define ACCELERATION_SOA      (R(0.1 * FRAME_RATE))

#define AVOID_FACTOR_SOA      (R(1.35 * FRAME_RATE))
#define MATCHING_FACTOR_SOA   (R(0.1 * FRAME_RATE))
#define CENTERING_FACTOR_SOA  (R(0.1 * FRAME_RATE))
#define BORDERING_FACTOR_SOA  (R(5.0 * FRAME_RATE))

#define POI_FACTOR_SOA        (R(10.0))

#define BOID_COUNT_SOA 	      (2000)

#define MAX_VISIBLE_SOA       (10)
#define VISION_RADIUS_SOA     (R(70.0))
#define PROTECTED_RANGE_SOA   (R(5.0))


#define BOID_TEXTURE_SOA      "gfx/boid.png"

typedef struct
{
	real* x;
	real* y;

	real* speedX;
	real* speedY;

	real* angle;
} BoidSOA;

typedef struct
{
	int numBoids;
	real topSpeed;
	real minSpeed;
	real turnSpeed;
	real acceleration;

	real avoidFactor;
	real matchingFactor;
	real centeringFactor;
	real borderingFactor;

	int maxVisible;
	real visionRadius;
	real visionRadiusSq;
	real protectedRange;
	real protectedRangeSq;
	real poiFactor;

	SDL_Texture* texture;

} SOASimulationParameters;

BoidSOA* boidsoa_create(SOASimulationParameters* sim);

void boidsoa_destroy(BoidSOA* boids);

void drawBoidSOA(BoidSOA* boid, int i, SOASimulationParameters* sim);


#endif