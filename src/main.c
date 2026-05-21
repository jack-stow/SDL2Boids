
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
Stats stats;


void displayFPS(double fps)
{
	char title[128];
	snprintf(title, sizeof(title), "FPS: %.2f", fps);

	SDL_SetWindowTitle(app.window, title);
}

void printStats(void)
{
	printf("\n--- Simulation Stats ---\n");

	printf("Runtime: %.2f sec\n", stats.runTime);

	printf("FPS min/avg/max: %.2f / %.2f / %.2f\n",
		stats.minFps,
		stats.fpsSum / stats.fpsSamples,
		stats.maxFps);

	printf("Update ms min/avg/max: %.4f / %.4f / %.4f\n",
		stats.minUpdateMs,
		stats.updateMsSum / stats.updateSamples,
		stats.maxUpdateMs);

	printf("Draw ms min/avg/max: %.4f / %.4f / %.4f\n",
		stats.minDrawMs,
		stats.drawMsSum / stats.drawSamples,
		stats.maxDrawMs);

	cleanup();
}

int main(int argc, char* argv[])
{
	memset(&app, 0, sizeof(App));
	memset(&player, 0, sizeof(Entity));
	srand(RNG_SEED);
	initSDL();

	//atexit(cleanup);
	atexit(printStats);
	double posX = 100.0;
	double posY = 100.0;


	SimulationParameters sim = {
		.topSpeed = TOP_SPEED,
		.minSpeed = MIN_SPEED,
		.turnSpeed = TURN_SPEED,
		.acceleration = ACCELERATION,

		.avoidFactor = AVOID_FACTOR,
		.matchingFactor = MATCHING_FACTOR,
		.centeringFactor = CENTERING_FACTOR,
		.borderingFactor = BORDERING_FACTOR,

		.maxVisible = MAX_VISIBLE,
		.visionRadius = VISION_RADIUS,
		.visionRadiusSq = VISION_RADIUS * VISION_RADIUS,
		.protectedRange = PROTECTED_RANGE,
		.protectedRangeSq = PROTECTED_RANGE * PROTECTED_RANGE,

		.poiFactor = POI_FACTOR
	};

	int boidCount = BOID_COUNT;

	Boid* boids = malloc(sizeof(Boid) * boidCount);

	for (size_t i = 0; i < boidCount; i++)
	{
		boids[i] = boid_create(sim, "gfx/boid.png");
	}

	int poiCount = NUM_POI;
	PointOfInterest* pointsOfInterest = malloc(sizeof(PointOfInterest) * poiCount);

	for (size_t i = 0; i < poiCount; i++)
	{
		pointsOfInterest[i] = poi_create_random();
	}

	initPlayer(&player, posX, posY, sim.topSpeed, sim.acceleration, "gfx/boid.png");
	Uint64 lastCounter = SDL_GetPerformanceCounter();

	double fpsTimer = 0.0;
	int frameCount = 0;

	Uint64 updateStart = 0;
	Uint64 updateEnd = 0;
	Uint64 drawEnd = 0;

	double flockTimeAccum = 0.0;
	int flockCallCount = 0;
	double statsTimer = 0.0;

	Color poiColor = { 0, 255, 0, 255 };


	// Statistics
	stats.runTime = 0.0;

	stats.minFps = DBL_MAX;
	stats.maxFps = 0.0;
	stats.fpsSum = 0.0;
	stats.fpsSamples = 0;

	stats.minUpdateMs = DBL_MAX;
	stats.maxUpdateMs = 0.0;
	stats.updateMsSum = 0.0;
	stats.updateSamples = 0;

	stats.minDrawMs = DBL_MAX;
	stats.maxDrawMs = 0.0;
	stats.drawMsSum = 0.0;
	stats.drawSamples = 0;

	double titleTimer = 0.0;
	int titleFrameCount = 0;

	double titleUpdateMsSum = 0.0;
	double titleDrawMsSum = 0.0;
	int titleSamples = 0;
	/////////////////////////

	while (1)
	{
		Uint64 frameStart = SDL_GetPerformanceCounter();

		double deltaTime =
			(double)(frameStart - lastCounter) /
			SDL_GetPerformanceFrequency();

		lastCounter = frameStart;

		prepareScene();
		doInput();

		Uint64 updateStart = SDL_GetPerformanceCounter();

		UpdateBoids(
			boids,
			boidCount,
			sim,
			pointsOfInterest,
			poiCount,
			deltaTime
		);

		Uint64 updateEnd = SDL_GetPerformanceCounter();

		DrawBoids(boids, boidCount);

		for (size_t i = 0; i < poiCount; i++)
		{
			if (!pointsOfInterest[i].active)
			{
				pointsOfInterest[i] = poi_reinitialize(&pointsOfInterest[i]);
			}

			poi_draw(&pointsOfInterest[i], poiColor);
		}

		draw_circle(
			(int)boids[0].x,
			(int)boids[0].y,
			sim.visionRadius,
			(Color) {
			255, 0, 255, 255
		},
			false
		);

		draw_circle(
			(int)boids[0].x,
			(int)boids[0].y,
			sim.protectedRange,
			(Color) {
			255, 0, 255, 255
		},
			false
		);

		Uint64 drawEnd = SDL_GetPerformanceCounter();

		double updateMs =
			(double)(updateEnd - updateStart) /
			SDL_GetPerformanceFrequency() * 1000.0;

		double drawMs =
			(double)(drawEnd - updateEnd) /
			SDL_GetPerformanceFrequency() * 1000.0;

		if (deltaTime > 0.0)
		{
			double fps = 1.0 / deltaTime;

			if (fps < stats.minFps) stats.minFps = fps;
			if (fps > stats.maxFps) stats.maxFps = fps;

			stats.fpsSum += fps;
			stats.fpsSamples++;
		}

		if (updateMs < stats.minUpdateMs) stats.minUpdateMs = updateMs;
		if (updateMs > stats.maxUpdateMs) stats.maxUpdateMs = updateMs;
		stats.updateMsSum += updateMs;
		stats.updateSamples++;

		if (drawMs < stats.minDrawMs) stats.minDrawMs = drawMs;
		if (drawMs > stats.maxDrawMs) stats.maxDrawMs = drawMs;
		stats.drawMsSum += drawMs;
		stats.drawSamples++;

		stats.runTime += deltaTime;

		titleTimer += deltaTime;
		titleFrameCount++;

		titleUpdateMsSum += updateMs;
		titleDrawMsSum += drawMs;
		titleSamples++;

		if (titleTimer >= 1.0)
		{
			char title[128];

			snprintf(
				title,
				sizeof(title),
				"FPS: %.1f | Update: %.3f ms | Draw: %.3f ms",
				(double)titleFrameCount / titleTimer,
				titleUpdateMsSum / titleSamples,
				titleDrawMsSum / titleSamples
			);

			SDL_SetWindowTitle(app.window, title);

			titleTimer = 0.0;
			titleFrameCount = 0;
			titleUpdateMsSum = 0.0;
			titleDrawMsSum = 0.0;
			titleSamples = 0;
		}

		presentScene();

		Uint64 frameEnd = SDL_GetPerformanceCounter();

		double frameSeconds =
			(double)(frameEnd - frameStart) /
			SDL_GetPerformanceFrequency();

		double targetFrameTime = 1.0 / 60.0;

		if (frameSeconds < targetFrameTime)
		{
			SDL_Delay((Uint32)((targetFrameTime - frameSeconds) * 1000.0));
		}
	}

	return 0;
}
