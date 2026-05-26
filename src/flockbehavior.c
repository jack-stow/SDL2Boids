#include "flockbehavior.h"

void DrawBoids(Boid* boids, int numBoids, SimulationParameters* sim) {
	for (size_t i = 0; i < numBoids; i++)
	{
		drawBoid(&boids[i], sim);
	}
}

void UpdateBoids(Boid* boids, int numBoids, SimulationParameters* sim, PointOfInterest* pointsOfInterest, int poiCount, real deltaTime) {
    
    for (size_t i = 0; i < numBoids; i++)
    {
        Flock(&boids[i], boids, numBoids, sim, pointsOfInterest, poiCount, deltaTime);

        boids[i].x += boids[i].speed.x * deltaTime;
        boids[i].y += boids[i].speed.y * deltaTime;


        if (boids[i].x < 0) boids[i].x += SCREEN_WIDTH;
        if (boids[i].x > SCREEN_WIDTH) boids[i].x -= SCREEN_WIDTH;

        if (boids[i].y < 0) boids[i].y += SCREEN_HEIGHT;
        if (boids[i].y > SCREEN_HEIGHT) boids[i].y -= SCREEN_HEIGHT;

    }
}

void Flock(Boid* boid, Boid* boids, int numBoids, SimulationParameters* sim, PointOfInterest* pointsOfInterest, int poiCount, real deltaTime) {
    vec2 avoid = { 0, 0 };
    vec2 align = { 0, 0 };
    vec2 cohere = { 0, 0 };

    int avoidCount = 0;
    int alignCount = 0;
    int cohesionCount = 0;

    vec2 boidPos = { boid->x, boid->y };

    for (int i = 0; i < numBoids; i++)
    {
        Boid* other = &boids[i];

        if (other == boid)
        {
            continue;
        }

        if (alignCount >= sim->maxVisible)
        {
            break;
        }

        vec2 otherPos = { other->x, other->y };

		real distanceSq = vec_dist_sq(boidPos, otherPos);
        if (distanceSq <= 0.0 || distanceSq >= sim->visionRadiusSq)
        {
            continue;
		}

        // Avoidance
        if (distanceSq < sim->protectedRangeSq)
        {
            vec2 away = vec_sub(boidPos, otherPos);

            // Stronger avoidance when very close
            away = vec_mul(away, ((real)1.0) / distanceSq);

            avoid = vec_add(avoid, away);
            avoidCount++;
        }

        // Alignment
        align = vec_add(align, other->speed);
        alignCount++;

        // Cohesion
        cohere = vec_add(cohere, otherPos);
        cohesionCount++;
    }

    if (avoidCount > 0)
    {
        avoid = vec_mul(avoid, R(1.0) / R(avoidCount));
        avoid = vec_norm(avoid);
    }

    if (alignCount > 0)
    {
        align = vec_mul(align, R(1.0) / R(alignCount));
        align = vec_sub(align, boid->speed);
        align = vec_norm(align);
    }

    if (cohesionCount > 0)
    {
        cohere = vec_mul(cohere, R(1.0) / R(cohesionCount));
        cohere = vec_sub(cohere, boidPos);
        cohere = vec_norm(cohere);
    }

    vec2 flockForce = { 0, 0 };

    flockForce = vec_add(flockForce, vec_mul(avoid, sim->avoidFactor));
    flockForce = vec_add(flockForce, vec_mul(align, sim->matchingFactor));
    flockForce = vec_add(flockForce, vec_mul(cohere, sim->centeringFactor));

    vec2 wallForce = AvoidBorders(boid, BORDER_MARGIN);
	vec2 poiForce = { 0, 0 };

    PointOfInterest* closestPOI = NULL;
    real closestDistSq = REAL_MAX;

    for (int i = 0; i < poiCount; i++)
    {
        if (!pointsOfInterest[i].active)
        {
            continue;
        }

        vec2 offset = poi_get_direction_vector(&pointsOfInterest[i], boid);
        real distSq = vec_mag_sq(offset);

        if (distSq < closestDistSq)
        {
            closestDistSq = distSq;
            closestPOI = &pointsOfInterest[i];
        }
    }
    if (closestPOI != NULL)
    {
        poiForce = poi_get_force(closestPOI, boid, sim);
        consume_poi(closestPOI, boid, 1);
    }

    flockForce = vec_add(flockForce, vec_mul(poiForce, sim->poiFactor));

    flockForce = vec_add(flockForce, vec_mul(wallForce, sim->borderingFactor));


    vec2 acceleration = vec_mul(flockForce, R(1.0) / sim->turnSpeed);

    boid->speed = vec_add(boid->speed, vec_mul(acceleration, deltaTime));

    boid->speed = vec_clamp_mag(
        boid->speed,
        R(sim->minSpeed),
        R(sim->topSpeed)
    );

    if (vec_mag(boid->speed) > 0.01)
    {
        boid->angle = REAL_ATAN2(boid->speed.y, boid->speed.x) * R(180.0) / REAL_PI;
    }
}


vec2 AvoidBorders(Boid* boid, real borderMargin)
{
    vec2 force = { 0, 0 };

    if (boid->x < borderMargin)
    {
        real t = (borderMargin - boid->x) / borderMargin;
        force.x += t * t;
    }
    else if (boid->x > SCREEN_WIDTH - borderMargin)
    {
        real t = (boid->x - (SCREEN_WIDTH - borderMargin)) / borderMargin;
        force.x -= t * t;
    }

    if (boid->y < borderMargin)
    {
        real t = (borderMargin - boid->y) / borderMargin;
        force.y += t * t;
    }
    else if (boid->y > SCREEN_HEIGHT - borderMargin)
    {
        real t = (boid->y - (SCREEN_HEIGHT - borderMargin)) / borderMargin;
        force.y -= t * t;
    }

    return force;
}