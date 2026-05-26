
#include "boid.h"
#include <time.h>

Boid boid_create(SimulationParameters* sim)
{
    Boid boid;

    boid.x = rand_range_real(BORDER_MARGIN, SCREEN_WIDTH - BORDER_MARGIN);
    boid.y = rand_range_real(BORDER_MARGIN, SCREEN_HEIGHT - BORDER_MARGIN);

    // Random direction
    vec2 dir = {rand_range_real(R(-1.0), R(1.0)), rand_range_real(R(-1.0), R(1.0))};
    
    dir = vec_norm(dir);

    // Random speed magnitude
    real speed = ((real)rand() / (real)RAND_MAX) * sim->topSpeed;

	speed = CLAMP(speed, sim->minSpeed, sim->topSpeed);

    boid.speed = vec_mul(dir, speed);

    boid.angle = REAL_ATAN2(boid.speed.y, boid.speed.x) * RAD_TO_DEG;

    return boid;
}

void drawBoid(Boid* boid, SimulationParameters* sim)
{
	blit(sim->texture, boid->x, boid->y, boid->angle, R(BOID_SCALE));
}
