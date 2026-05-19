
#include "boid.h"
#include <time.h>

Boid boid_create(double topSpeed, double minSpeed, double acceleration, char* texture)
{
    Boid boid;

    boid.x = rand_range_double(50.0, SCREEN_WIDTH - 50.0);
    boid.y = rand_range_double(50.0, SCREEN_HEIGHT - 50.0);

    // Random direction
    vec2 dir = {
        ((double)rand() / RAND_MAX) * 2.0 - 1.0,
        ((double)rand() / RAND_MAX) * 2.0 - 1.0
    };

    dir = vec_norm(dir);

    // Random speed magnitude
    double speed = ((double)rand() / RAND_MAX) * topSpeed;

	speed = CLAMP(speed, minSpeed, topSpeed);

    boid.speed = vec_mul(dir, speed);
    boid.desiredSpeed = boid.speed;

    boid.angle = atan2(boid.speed.y, boid.speed.x) * 180.0 / M_PI;

    boid.desiredAngle = boid.angle;

    boid.topSpeed = topSpeed;
    boid.acceleration = acceleration;

    boid.texture = loadTexture(texture);

    return boid;
}

void drawBoid(Boid* boid)
{
	blit(boid->texture, boid->x, boid->y, boid->angle, BOID_SCALE);
}
