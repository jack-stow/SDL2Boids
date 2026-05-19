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
#include "poi.h"

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
	double posX = 100.0;
	double posY = 100.0;


	SimulationParameters sim = {
		.topSpeed = TOP_SPEED,
		.minSpeed = MIN_SPEED,
		.acceleration = ACCELERATION,

		.avoidFactor = AVOID_FACTOR,
		.matchingFactor = MATCHING_FACTOR,
		.centeringFactor = CENTERING_FACTOR,
		.borderingFactor = BORDERING_FACTOR,

		.maxVisible = MAX_VISIBLE,
		.visionRadius = VISION_RADIUS,
		.protectedRange = PROTECTED_RANGE,

		.poiFactor = POI_FACTOR
	};

	int boidCount = BOID_COUNT;

	Boid* boids = malloc(sizeof(Boid) * boidCount);

	for (size_t i = 0; i < boidCount; i++)
	{
		boids[i] = boid_create(sim, "gfx/boid.png");
	}

	int poiCount = 5;
	PointOfInterest* pointsOfInterest = malloc(sizeof(PointOfInterest) * poiCount);

	for (size_t i = 0; i < poiCount; i++)
	{
		pointsOfInterest[i] = poi_create_random();
	}

	initPlayer(&player, posX, posY, sim.topSpeed, sim.acceleration, "gfx/boid.png");
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
		Uint64 frameStart = SDL_GetPerformanceCounter();
		prepareScene();
		doInput();
		/*

		updatePlayer(&player, (vec2) {
			(double)(app.right - app.left),
				(double)(app.down - app.up)
		});

		drawPlayer(&player);*/


		flockStart = SDL_GetPerformanceCounter();
		HandleBoids(boids, boidCount, sim, pointsOfInterest, poiCount, deltaTime);

		for (size_t i = 0; i < poiCount; i++)
		{
			if (!pointsOfInterest[i].active) {
				pointsOfInterest[i] = poi_reinitialize(&pointsOfInterest[i]);
			}

			poi_draw(&pointsOfInterest[i]);
		}

		flockEnd = SDL_GetPerformanceCounter();
		double flockSeconds =
			(double)(flockEnd - flockStart) / SDL_GetPerformanceFrequency();

		flockTimeAccum += flockSeconds;
		flockCallCount++;
		statsTimer += deltaTime;
		frameCount++;
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

		//draw_circle(player.x, player.y, 10.0);


		presentScene();

		Uint64 frameEnd = SDL_GetPerformanceCounter();

		double frameSeconds =
			(double)(frameEnd - frameStart) / SDL_GetPerformanceFrequency();

		double targetFrameTime = 1.0 / 60.0;

		if (frameSeconds < targetFrameTime)
		{
			SDL_Delay((Uint32)((targetFrameTime - frameSeconds) * 1000.0));
		}

		//SDL_Delay(16);
		
	}

	return 0;
}
