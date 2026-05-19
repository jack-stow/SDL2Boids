#include "flockbehavior.h"


void HandleBoids(Boid* boids, int numBoids, double avoidFactor, double matchingFactor, double centeringFactor, double borderingFactor, int maxVisible, double visionRadius, double protectedRange, double deltaTime) {
    

    for (size_t i = 0; i < numBoids; i++)
    {
        Flock(&boids[i], boids, numBoids, avoidFactor, matchingFactor, centeringFactor, borderingFactor, maxVisible, visionRadius, protectedRange, deltaTime);

        boids[i].x += boids[i].speed.x;
        boids[i].y += boids[i].speed.y;

        if (boids[i].x < 0) boids[i].x += SCREEN_WIDTH;
        if (boids[i].x > SCREEN_WIDTH) boids[i].x -= SCREEN_WIDTH;

        if (boids[i].y < 0) boids[i].y += SCREEN_HEIGHT;
        if (boids[i].y > SCREEN_HEIGHT) boids[i].y -= SCREEN_HEIGHT;

        drawBoid(&boids[i]);
    }
}

void Flock(Boid* boid, Boid* boids, int numBoids, double avoidFactor, double matchingFactor, double centeringFactor, double borderingFactor, int maxVisible, double visionRadius, double protectedRange, double deltaTime) {
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
        align = vec_add(align, other->speed);
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
        align = vec_sub(align, boid->speed);
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
    flockForce = vec_add(flockForce, vec_mul(align, matchingFactor));
    flockForce = vec_add(flockForce, vec_mul(cohere, centeringFactor));


    //vec2 borderForce = AvoidBorders(boid, 100.0);

    vec2 desiredSpeed = boid->speed;

    desiredSpeed = vec_add(desiredSpeed, vec_mul(flockForce, boid->acceleration));

   /* desiredSpeed = vec_add(
        desiredSpeed,
        vec_mul(borderForce, borderingFactor)
    );*/



    if (vec_mag(desiredSpeed) > boid->topSpeed)
    {
        desiredSpeed = vec_mul(
            vec_norm(desiredSpeed),
            boid->topSpeed
        );
    }

    boid->desiredSpeed = desiredSpeed;

    boid->speed = vec_lerp(
        boid->speed,
        boid->desiredSpeed,
        boid->acceleration
    );



    boid->speed = vec_clamp_mag(boid->speed, boid->minSpeed, boid->topSpeed);

    if (vec_mag(boid->speed) > 0.01)
    {
        boid->desiredAngle = atan2(boid->speed.y, boid->speed.x) * 180.0 / M_PI;
    }


    boid->angle = normalize_angle(
        angle_lerp(boid->angle, boid->desiredAngle, 0.25)
    );
}


vec2 AvoidBorders(Boid* boid, double borderMargin)
{
    vec2 force = { 0, 0 };

    if (boid->x < borderMargin)
    {
        double t = (borderMargin - boid->x) / borderMargin;
        force.x += t * t;
    }
    else if (boid->x > SCREEN_WIDTH - borderMargin)
    {
        double t = (boid->x - (SCREEN_WIDTH - borderMargin)) / borderMargin;
        force.x -= t * t;
    }

    if (boid->y < borderMargin)
    {
        double t = (borderMargin - boid->y) / borderMargin;
        force.y += t * t;
    }
    else if (boid->y > SCREEN_HEIGHT - borderMargin)
    {
        double t = (boid->y - (SCREEN_HEIGHT - borderMargin)) / borderMargin;
        force.y -= t * t;
    }

    return force;
}