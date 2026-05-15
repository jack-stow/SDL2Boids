/*
 * Copyright (C) 2015-2018,2022 Parallel Realities. All rights reserved.
 */

#include "common.h"

#include "draw.h"
#include "init.h"
#include "input.h"
#include "main.h"
#include "player.h"
#include "boid.h"
#include "flockbehavior.h"

App    app;
Entity player;

int main(int argc, char* argv[])
{
	memset(&app, 0, sizeof(App));
	memset(&player, 0, sizeof(Entity));

	initSDL();

	atexit(cleanup);

	int topSpeed = 8;
	double posX = 100.0;
	double posY = 100.0;
	double acceleration = 0.05;
	vec2 speedNormalized = { 0, 0 };
	int boidCount = 10;
	Boid* boids = malloc(sizeof(Boid) * boidCount);

	double avoidFactor = 0.05;
	double matchingFactor = 0.05;
	double centeringFactor = 0.001;	

	int maxVisible = 10;
	double visionRadius = 100.0;
	double protectedRange = 20.0;


	for (size_t i = 0; i < boidCount; i++)
	{
		boids[i] = boid_create(topSpeed, acceleration, "gfx/boid.png");
	}

	initPlayer(&player, posX, posY, topSpeed, acceleration, "gfx/boid.png");
	Uint64 lastCounter = SDL_GetPerformanceCounter();
	while (1)
	{
		Uint64 currentCounter = SDL_GetPerformanceCounter();

		double deltaTime =
			(double)(currentCounter - lastCounter) /
			SDL_GetPerformanceFrequency();

		lastCounter = currentCounter;

		prepareScene();

		doInput();

		updatePlayer(&player, (vec2) {
			(double)(app.right - app.left),
				(double)(app.down - app.up)
		});

		drawPlayer(&player);

		for (size_t i = 0; i < boidCount; i++)
		{
			Flock(&boids[i], boids, boidCount, avoidFactor, matchingFactor, centeringFactor, maxVisible, visionRadius, protectedRange, deltaTime);

			boids[i].x += boids[i].speed.x;
			boids[i].y += boids[i].speed.y;

			drawBoid(&boids[i]);	
		}



		presentScene();

		SDL_Delay(16);
		
	}

	return 0;
}
