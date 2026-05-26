#include "flockbehaviorSOA.h"


void DrawBoidsSOA(BoidSOA* boids, SOASimulationParameters* sim) {
    for (size_t i = 0; i < sim->numBoids; i++)
    {
        drawBoidSOA(boids, i, sim);
    }
}



void UpdateBoidsSOA(BoidSOA* boids, SOASimulationParameters* sim, PointOfInterest* pointsOfInterest, int poiCount, real deltaTime) {
    for (size_t i = 0; i < sim->numBoids; i++)
    {
        FlockSOA(boids, i, sim, pointsOfInterest, poiCount, deltaTime);
        boids->x[i] += boids->speedX[i] * deltaTime;
        boids->y[i] += boids->speedY[i] * deltaTime;

		if (boids->x[i] < 0) boids->x[i] += SCREEN_WIDTH;
		if (boids->x[i] > SCREEN_WIDTH) boids->x[i] -= SCREEN_WIDTH;

		if (boids->y[i] < 0) boids->y[i] += SCREEN_HEIGHT;
		if (boids->y[i] > SCREEN_HEIGHT) boids->y[i] -= SCREEN_HEIGHT;
    }
}

void FlockSOA(BoidSOA* boidSOA, int boidIndex, SOASimulationParameters* sim, PointOfInterest* pointsOfInterest, int poiCount, real deltaTime) {

    vec2 avoid = { 0, 0 };
    vec2 align = { 0, 0 };
    vec2 cohere = { 0, 0 };


    int avoidCount = 0;
    int alignCount = 0;
    int cohesionCount = 0;
    

    for (int i = 0; i < sim->numBoids; i++)
    {
        if (i == boidIndex)
        {
            continue;
        }

        if (alignCount >= sim->maxVisible)
        {
            break;
        }

        real distanceSq = vec_dist_sq_xy(
            boidSOA->x[boidIndex],
            boidSOA->y[boidIndex],
            boidSOA->x[i],
            boidSOA->y[i]
        );
        if (distanceSq <= 0.0 || distanceSq >= sim->visionRadiusSq)
        {
            continue;
        }

		// Avoidance
		if (distanceSq < sim->protectedRangeSq)
		{
			real awayX = boidSOA->x[boidIndex] - boidSOA->x[i];
			real awayY = boidSOA->y[boidIndex] - boidSOA->y[i];

			// Stronger avoidance when very close
			awayX *= ((real)1.0) / distanceSq;
			awayY *= ((real)1.0) / distanceSq;

			avoid.x += awayX;
			avoid.y += awayY;
			
            avoidCount++;
		}

		// Alignment

		align.x += boidSOA->speedX[i];
		align.y += boidSOA->speedY[i];
		alignCount++;

		// Cohesion

		cohere.x += boidSOA->x[i];
		cohere.y += boidSOA->y[i];
		cohesionCount++;

    }

	if (avoidCount > 0)
	{
		avoid.x *= R(1.0) / (real)avoidCount;
		avoid.y *= R(1.0) / (real)avoidCount;
		real mag = vec_mag_xy(avoid.x, avoid.y);
		if (mag > 0)
		{
			avoid.x /= mag;
			avoid.y /= mag;
		}
	}
	if (alignCount > 0)
	{
		align.x *= R(1.0) / (real)alignCount;
		align.y *= R(1.0) / (real)alignCount;

		align.x -= boidSOA->speedX[boidIndex];
		align.y -= boidSOA->speedY[boidIndex];

		vec_norm_xy(align.x, align.y, &align.x, &align.y);
	}

	if (cohesionCount > 0)
	{
		cohere.x *= R(1.0) / (real)cohesionCount;
		cohere.y *= R(1.0) / (real)cohesionCount;

		cohere.x -= boidSOA->x[boidIndex];
		cohere.y -= boidSOA->y[boidIndex];

		vec_norm_xy(cohere.x, cohere.y, &cohere.x, &cohere.y);
	}

	vec2 flockForce = { 0, 0 };

	flockForce.x += avoid.x * sim->avoidFactor;
	flockForce.y += avoid.y * sim->avoidFactor;

	flockForce.x += align.x * sim->matchingFactor;
	flockForce.y += align.y * sim->matchingFactor;

	flockForce.x += cohere.x * sim->centeringFactor;
	flockForce.y += cohere.y * sim->centeringFactor;

	vec2 wallForce = AvoidBordersSOA(boidSOA, boidIndex, BORDER_MARGIN);

	vec2 poiForce = { 0, 0 };

	PointOfInterest* closestPOI = NULL;
	real closestDistSq = REAL_MAX;
	for (int i = 0; i < poiCount; i++)
	{
		if (!pointsOfInterest[i].active)
		{
			continue;
		}
		real offsetX = pointsOfInterest[i].x - boidSOA->x[boidIndex];
		real offsetY = pointsOfInterest[i].y - boidSOA->y[boidIndex];
		real distSq = offsetX * offsetX + offsetY * offsetY;
		if (distSq < closestDistSq)
		{
			closestDistSq = distSq;
			closestPOI = &pointsOfInterest[i];
		}
	}
	if (closestPOI != NULL)
	{
		poiForce = poi_get_force_soa(closestPOI, boidSOA, boidIndex, sim);
		consume_poi_soa(closestPOI, boidSOA, boidIndex, 1);
	}

	flockForce = vec_add(flockForce, vec_mul(poiForce, sim->poiFactor));

	flockForce = vec_add(flockForce, vec_mul(wallForce, sim->borderingFactor));

	vec2 acceleration = vec_mul(flockForce, R(1.0) / sim->turnSpeed);

	boidSOA->speedX[boidIndex] += acceleration.x * deltaTime;
	boidSOA->speedY[boidIndex] += acceleration.y * deltaTime;

	vec_clamp_mag_xy(
		boidSOA->speedX[boidIndex],
		boidSOA->speedY[boidIndex],
		sim->minSpeed,
		sim->topSpeed,
		&boidSOA->speedX[boidIndex],
		&boidSOA->speedY[boidIndex]
	);

	if (vec_mag_xy(boidSOA->speedX[boidIndex], boidSOA->speedY[boidIndex]) > 0.01)
	{
		boidSOA->angle[boidIndex] = REAL_ATAN2(boidSOA->speedY[boidIndex], boidSOA->speedX[boidIndex]) * R(180.0) / REAL_PI;
	}
	
}

vec2 AvoidBordersSOA(BoidSOA* boidSOA, int boidIndex, real borderMargin) {
    vec2 force = { 0, 0 };

    if (boidSOA->x[boidIndex] < borderMargin)
    {
        real t = (borderMargin - boidSOA->x[boidIndex]) / borderMargin;
        force.x += t * t;
    }
    else if (boidSOA->x[boidIndex] > SCREEN_WIDTH - borderMargin)
    {
        real t = (boidSOA->x[boidIndex] - (SCREEN_WIDTH - borderMargin)) / borderMargin;
        force.x -= t * t;
    }

    if (boidSOA->y[boidIndex] < borderMargin)
    {
        real t = (borderMargin - boidSOA->y[boidIndex]) / borderMargin;
        force.y += t * t;
    }
    else if (boidSOA->y[boidIndex] > SCREEN_HEIGHT - borderMargin)
    {
        real t = (boidSOA->y[boidIndex] - (SCREEN_HEIGHT - borderMargin)) / borderMargin;
        force.y -= t * t;
    }

    return force;
}

