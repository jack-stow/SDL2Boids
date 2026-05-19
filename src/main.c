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


void displayFPS(double fps)
{
	char title[128];
	snprintf(title, sizeof(title), "FPS: %.2f", fps);

	SDL_SetWindowTitle(app.window, title);
}

int main(int argc, char* argv[])
{
	memset(&app, 0, sizeof(App));
	memset(&player, 0, sizeof(Entity));
	srand((unsigned int)time(NULL));
	initSDL();

	atexit(cleanup);

	double topSpeed = 5.0;
	double minSpeed = topSpeed * 0.4;
	double posX = 100.0;
	double posY = 100.0;
	double acceleration = 0.08;
	vec2 speedNormalized = { 0, 0 };
	int boidCount = 500;
	Boid* boids = malloc(sizeof(Boid) * boidCount);

	double avoidFactor = 3;
	double matchingFactor = 3;
	double centeringFactor = 3;
	double borderingFactor = 8.0;

	int maxVisible = 20;
	double visionRadius = 100.0;
	double protectedRange = 30.0;


	for (size_t i = 0; i < boidCount; i++)
	{
		boids[i] = boid_create(topSpeed, minSpeed, acceleration, "gfx/boid.png");
	}

	initPlayer(&player, posX, posY, topSpeed, acceleration, "gfx/boid.png");
	Uint64 lastCounter = SDL_GetPerformanceCounter();

	double fpsTimer = 0.0;
	int frameCount = 0;

	Uint64 flockStart = 0;
	Uint64 flockEnd = 0;

	double flockTimeAccum = 0.0;
	int flockCallCount = 0;
	double statsTimer = 0.0;

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


		flockStart = SDL_GetPerformanceCounter();
		HandleBoids(boids, boidCount, avoidFactor, matchingFactor, centeringFactor, borderingFactor, maxVisible, visionRadius, protectedRange, deltaTime);
		flockEnd = SDL_GetPerformanceCounter();
		double flockSeconds =
			(double)(flockEnd - flockStart) / SDL_GetPerformanceFrequency();

		flockTimeAccum += flockSeconds;
		flockCallCount++;
		statsTimer += deltaTime;
		if (statsTimer >= 1.0)
		{
			double avgMs = (flockTimeAccum / flockCallCount) * 1000.0;

			char title[128];
			snprintf(title, sizeof(title),
				"FPS: %.1f | HandleBoids avg: %.3f ms",
				(double)frameCount / statsTimer,
				avgMs
			);

			SDL_SetWindowTitle(app.window, title);

			flockTimeAccum = 0.0;
			flockCallCount = 0;
			statsTimer = 0.0;
			frameCount = 0;
		}


		presentScene();

		SDL_Delay(16);
		
	}

	return 0;
}
