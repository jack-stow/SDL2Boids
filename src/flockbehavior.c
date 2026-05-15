#include "flockbehavior.h"

void Flock(Boid* boid, Boid* boids, int numBoids, double avoidFactor, double matchingFactor, double centeringFactor, int maxVisible, double visionRadius, double protectedRange, double deltaTime) {
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

        if (alignCount >= maxVisible)
        {
            break;
        }

        vec2 otherPos = { other->x, other->y };

        double distance = vec_dist(boidPos, otherPos);

        if (distance <= 0.0 || distance >= visionRadius)
        {
            continue;
        }

        // Avoidance
        if (distance < protectedRange)
        {
            vec2 away = vec_sub(boidPos, otherPos);

            // Stronger avoidance when very close
            away = vec_mul(away, 1.0 / (distance * distance));

            avoid = vec_add(avoid, away);
            avoidCount++;
        }

        // Alignment
        align = vec_add(align, other->desiredSpeed);
        alignCount++;

        // Cohesion
        cohere = vec_add(cohere, otherPos);
        cohesionCount++;
    }

    if (avoidCount > 0)
    {
        avoid = vec_mul(avoid, 1.0 / avoidCount);
        avoid = vec_norm(avoid);
    }

    if (alignCount > 0)
    {
        align = vec_mul(align, 1.0 / alignCount);
        align = vec_sub(align, boid->desiredSpeed);
        align = vec_norm(align);
    }

    if (cohesionCount > 0)
    {
        cohere = vec_mul(cohere, 1.0 / cohesionCount);
        cohere = vec_sub(cohere, boidPos);
        cohere = vec_norm(cohere);
    }

    vec2 flockForce = { 0, 0 };

    flockForce = vec_add(flockForce, vec_mul(avoid, avoidFactor));
    flockForce = vec_add(flockForce, vec_mul(align, 1.0 / matchingFactor));
    flockForce = vec_add(flockForce, vec_mul(cohere, 1.0 / centeringFactor));

    boid->desiredSpeed = vec_add(boid->desiredSpeed, flockForce);

    if (vec_mag(boid->desiredSpeed) > boid->topSpeed)
    {
        boid->desiredSpeed = vec_mul(vec_norm(boid->desiredSpeed), boid->topSpeed);
    }

    boid->speed = vec_lerp(boid->speed, boid->desiredSpeed, boid->acceleration * deltaTime);
}


vec2 AvoidBorders(Boid* boid, double borderMargin)
{
    vec2 acceleration = { 0, 0 };

    if (boid->x < borderMargin)
    {
        acceleration.x = 1.0;
    }
    else if (boid->x > SCREEN_WIDTH - borderMargin)
    {
        acceleration.x = -1.0;
    }

    if (boid->y < borderMargin)
    {
        acceleration.y = 1.0;
    }
    else if (boid->y > SCREEN_HEIGHT - borderMargin)
    {
        acceleration.y = -1.0;
    }

    return vec_norm(acceleration);
}