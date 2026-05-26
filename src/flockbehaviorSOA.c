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
void FlockSOA(
    BoidSOA* boidSOA,
    int boidIndex,
    SOASimulationParameters* sim,
    PointOfInterest* pointsOfInterest,
    int poiCount,
    real deltaTime)
{
    real myX = boidSOA->x[boidIndex];
    real myY = boidSOA->y[boidIndex];
    real mySpeedX = boidSOA->speedX[boidIndex];
    real mySpeedY = boidSOA->speedY[boidIndex];

    real avoidX = R(0.0);
    real avoidY = R(0.0);

    real alignX = R(0.0);
    real alignY = R(0.0);

    real cohereX = R(0.0);
    real cohereY = R(0.0);

    int avoidCount = 0;
    int alignCount = 0;
    int cohesionCount = 0;

    for (int i = 0; i < sim->numBoids; i++)
    {
        if (i == boidIndex)
            continue;

        if (alignCount >= sim->maxVisible)
            break;

        real otherX = boidSOA->x[i];
        real otherY = boidSOA->y[i];

        real dx = myX - otherX;
        real dy = myY - otherY;

        real distanceSq = dx * dx + dy * dy;

        if (distanceSq <= R(0.0) || distanceSq >= sim->visionRadiusSq)
            continue;

        if (distanceSq < sim->protectedRangeSq)
        {
            real invDistanceSq = R(1.0) / distanceSq;

            avoidX += dx * invDistanceSq;
            avoidY += dy * invDistanceSq;

            avoidCount++;
        }

        alignX += boidSOA->speedX[i];
        alignY += boidSOA->speedY[i];
        alignCount++;

        cohereX += otherX;
        cohereY += otherY;
        cohesionCount++;
    }

    if (avoidCount > 0)
    {
        real invAvoidCount = R(1.0) / (real)avoidCount;

        avoidX *= invAvoidCount;
        avoidY *= invAvoidCount;

        vec_norm_xy(avoidX, avoidY, &avoidX, &avoidY);
    }

    if (alignCount > 0)
    {
        real invAlignCount = R(1.0) / (real)alignCount;

        alignX *= invAlignCount;
        alignY *= invAlignCount;

        alignX -= mySpeedX;
        alignY -= mySpeedY;

        vec_norm_xy(alignX, alignY, &alignX, &alignY);
    }

    if (cohesionCount > 0)
    {
        real invCohesionCount = R(1.0) / (real)cohesionCount;

        cohereX *= invCohesionCount;
        cohereY *= invCohesionCount;

        cohereX -= myX;
        cohereY -= myY;

        vec_norm_xy(cohereX, cohereY, &cohereX, &cohereY);
    }

    real flockForceX = R(0.0);
    real flockForceY = R(0.0);

    flockForceX += avoidX * sim->avoidFactor;
    flockForceY += avoidY * sim->avoidFactor;

    flockForceX += alignX * sim->matchingFactor;
    flockForceY += alignY * sim->matchingFactor;

    flockForceX += cohereX * sim->centeringFactor;
    flockForceY += cohereY * sim->centeringFactor;

    real wallForceX = R(0.0);
    real wallForceY = R(0.0);

    if (myX < BORDER_MARGIN)
    {
        real t = (BORDER_MARGIN - myX) / BORDER_MARGIN;
        wallForceX += t * t;
    }
    else if (myX > SCREEN_WIDTH - BORDER_MARGIN)
    {
        real t = (myX - (SCREEN_WIDTH - BORDER_MARGIN)) / BORDER_MARGIN;
        wallForceX -= t * t;
    }

    if (myY < BORDER_MARGIN)
    {
        real t = (BORDER_MARGIN - myY) / BORDER_MARGIN;
        wallForceY += t * t;
    }
    else if (myY > SCREEN_HEIGHT - BORDER_MARGIN)
    {
        real t = (myY - (SCREEN_HEIGHT - BORDER_MARGIN)) / BORDER_MARGIN;
        wallForceY -= t * t;
    }

    flockForceX += wallForceX * sim->borderingFactor;
    flockForceY += wallForceY * sim->borderingFactor;

    
    // Keep POI disabled until base flocking is confirmed stable.
    real poiForceX = R(0.0);
    real poiForceY = R(0.0);

    PointOfInterest* closestPOI = NULL;
    real closestDistSq = REAL_MAX;

    for (int i = 0; i < poiCount; i++)
    {
        if (!pointsOfInterest[i].active)
            continue;

        real offsetX = pointsOfInterest[i].x - myX;
        real offsetY = pointsOfInterest[i].y - myY;
        real distSq = offsetX * offsetX + offsetY * offsetY;

        if (distSq < closestDistSq)
        {
            closestDistSq = distSq;
            closestPOI = &pointsOfInterest[i];
        }
    }

    if (closestPOI != NULL)
    {
        vec2 poiForce = poi_get_force_soa(closestPOI, boidSOA, boidIndex, sim);

        poiForceX = poiForce.x;
        poiForceY = poiForce.y;

        consume_poi_soa(closestPOI, boidSOA, boidIndex, 1);
    }

    flockForceX += poiForceX * sim->poiFactor;
    flockForceY += poiForceY * sim->poiFactor;
    

    real invTurnSpeed = R(1.0) / sim->turnSpeed;

    real accelerationX = flockForceX * invTurnSpeed;
    real accelerationY = flockForceY * invTurnSpeed;

    mySpeedX += accelerationX * deltaTime;
    mySpeedY += accelerationY * deltaTime;

    vec_clamp_mag_xy(
        mySpeedX,
        mySpeedY,
        sim->minSpeed,
        sim->topSpeed,
        &mySpeedX,
        &mySpeedY
    );

    boidSOA->speedX[boidIndex] = mySpeedX;
    boidSOA->speedY[boidIndex] = mySpeedY;

    if (vec_mag_sq_xy(mySpeedX, mySpeedY) > R(0.0001))
    {
        boidSOA->angle[boidIndex] =
            REAL_ATAN2(mySpeedY, mySpeedX) * RAD_TO_DEG;
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

