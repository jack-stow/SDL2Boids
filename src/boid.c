
#include "boid.h"
#include <time.h>

Boid boid_create(SimulationParameters sim, char* texture)
{
    Boid boid;

    boid.x = rand_range_real(50.0, SCREEN_WIDTH - 50.0);
    boid.y = rand_range_real(50.0, SCREEN_HEIGHT - 50.0);

    // Random direction
    vec2 dir = {rand_range_real(R(-1.0), R(1.0)), rand_range_real(R(-1.0), R(1.0))};
    
    dir = vec_norm(dir);

    // Random speed magnitude
    real speed = ((real)rand() / (real)RAND_MAX) * sim.topSpeed;

	speed = CLAMP(speed, sim.minSpeed, sim.topSpeed);

    boid.speed = vec_mul(dir, speed);
    boid.desiredSpeed = boid.speed;

    boid.angle = REAL_ATAN2(boid.speed.y, boid.speed.x) * R(180.0) / REAL_PI;

    boid.desiredAngle = boid.angle;

    boid.texture = loadTexture(texture);

    return boid;
}

void drawBoid(Boid* boid)
{
	blit(boid->texture, boid->x, boid->y, boid->angle, R(BOID_SCALE));
}
