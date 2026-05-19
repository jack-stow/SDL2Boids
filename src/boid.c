
#include "boid.h"
#include <time.h>

Boid boid_create(double topSpeed, double minSpeed, double acceleration, char* texture)
{
    Boid boid;

    /*boid.x = (double)(rand() % SCREEN_WIDTH);
    boid.y = (double)(rand() % SCREEN_HEIGHT);*/
    
    //boid.x = ((double)rand() / RAND_MAX) * 200.0;
	//boid.y = ((double)rand() / RAND_MAX) * 200.0;

    //boid.x = (SCREEN_WIDTH / 2.0) + ((double)rand() / RAND_MAX) * 200.0 - 100.0;
	//boid.y = (SCREEN_HEIGHT / 2.0) + ((double)rand() / RAND_MAX) * 200.0 - 100.0;

    boid.x = 300 + rand_range(-100, 100);
    boid.y = 300 + rand_range(-100, 100);

    // Random direction
    /*vec2 dir = {
        ((double)rand() / RAND_MAX) * 2.0 - 1.0,
        ((double)rand() / RAND_MAX) * 2.0 - 1.0
    };*/

    /*vec2 dir = {
    1.0,
    rand_range_double(-0.6, 0.6)
    };*/
    vec2 dir = {
    rand_range_double(0.6, 1.0),
    rand_range_double(-0.8, 0.8)
    };


    dir = vec_norm(dir);

    // Random speed magnitude
    double speed = ((double)rand() / RAND_MAX) * topSpeed;

	speed = CLAMP(speed, minSpeed, topSpeed);

    boid.speed = vec_mul(dir, speed);
    boid.desiredSpeed = boid.speed;

    boid.angle =
        atan2(boid.speed.y, boid.speed.x) * 180.0 / M_PI;

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
