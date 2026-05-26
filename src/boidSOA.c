
#include "boidSOA.h"
#include <time.h>


BoidSOA* boidsoa_create(SOASimulationParameters* sim)
{
    BoidSOA* boids = malloc(sizeof(BoidSOA));

    if (boids == NULL)
    {
        SDL_Log("Failed to allocate BoidSOA");
        exit(1);
    }

    boids->x = malloc(sizeof(real) * sim->numBoids);
    boids->y = malloc(sizeof(real) * sim->numBoids);
    boids->speedX = malloc(sizeof(real) * sim->numBoids);
    boids->speedY = malloc(sizeof(real) * sim->numBoids);
    boids->angle = malloc(sizeof(real) * sim->numBoids);

    if (boids->x == NULL || boids->y == NULL ||
        boids->speedX == NULL || boids->speedY == NULL ||
        boids->angle == NULL)
    {
        SDL_Log("Failed to allocate BoidSOA arrays");
        exit(1);
    }

    for (int i = 0; i < sim->numBoids; i++)
    {
        boids->x[i] = rand_range_real(BORDER_MARGIN, SCREEN_WIDTH - BORDER_MARGIN);
        boids->y[i] = rand_range_real(BORDER_MARGIN, SCREEN_HEIGHT - BORDER_MARGIN);

        vec2 dir = {
            rand_range_real(R(-1.0), R(1.0)),
            rand_range_real(R(-1.0), R(1.0))
        };

        dir = vec_norm(dir);

        real speed = rand_range_real(sim->minSpeed, sim->topSpeed);

        boids->speedX[i] = dir.x * speed;
        boids->speedY[i] = dir.y * speed;

        boids->angle[i] =
            REAL_ATAN2(boids->speedY[i], boids->speedX[i]) * RAD_TO_DEG;
    }

    return boids;
}

void boidsoa_destroy(BoidSOA* boids)
{
    if (boids == NULL)
        return;

    free(boids->x);
    free(boids->y);
    free(boids->speedX);
    free(boids->speedY);
    free(boids->angle);

    free(boids);
}

void drawBoidSOA(BoidSOA* boid, int boidIndex, SOASimulationParameters* sim) {
    blit(sim->texture, boid->x[boidIndex], boid->y[boidIndex], boid->angle[boidIndex], R(BOID_SCALE));
}
