
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

	#ifdef _DEBUG
		printf("Build: Debug\n");
	#else
		printf("Build: Release\n");
	#endif

	printf("Real type: %s\n", REAL_TYPE_NAME);

	printf("Boids: %d\n", BOID_COUNT);
	printf("POIs: %d\n", NUM_POI);
	printf("Seed: %d\n", RNG_SEED);
	printf("Benchmark Frames: %d\n", BENCHMARK_FRAMES);
	printf("Warm-up Frames: %d\n", WARMUP_FRAMES);

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

	printf("Frame work ms min/avg/max: %.4f / %.4f / %.4f\n",
		stats.minFrameWorkMs,
		stats.frameWorkMsSum / stats.frameWorkSamples,
		stats.maxFrameWorkMs);


	printf("\n------------------------\n");

	cleanup();
}

void UpdateStats(double deltaTime, Uint64 updateStart, Uint64 updateEnd, Uint64 drawEnd, Uint64 frameStart, Uint64 frameEnd, Uint64 performanceFreq) {

	static double titleTimer = 0.0;
	static int titleFrameCount = 0;
	static double titleUpdateMsSum = 0.0;
	static double titleDrawMsSum = 0.0;
	static int titleSamples = 0;

	double updateMs =
		(double)(updateEnd - updateStart) / performanceFreq * 1000.0;

	double drawMs =
		(double)(drawEnd - updateEnd) / performanceFreq * 1000.0;

	double presentMs =
		(double)(frameEnd - drawEnd) / performanceFreq * 1000.0;

	double frameWorkMs =
		(double)(frameEnd - frameStart) / performanceFreq * 1000.0;


	stats.runTime += deltaTime;

	if (stats.totalFrames > WARMUP_FRAMES) {

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

		titleTimer += deltaTime;
		titleFrameCount++;

		titleUpdateMsSum += updateMs;
		titleDrawMsSum += drawMs;
		titleSamples++;

		if (frameWorkMs < stats.minFrameWorkMs)
			stats.minFrameWorkMs = frameWorkMs;

		if (frameWorkMs > stats.maxFrameWorkMs)
			stats.maxFrameWorkMs = frameWorkMs;

		stats.frameWorkMsSum += frameWorkMs;
		stats.frameWorkSamples++;

		int framesRemaining = BENCHMARK_FRAMES - stats.totalFrames;
		double secondsRemaining = framesRemaining / 60.0;

		if (titleTimer >= 1.0)
		{
			char title[128];

			snprintf(
				title,
				sizeof(title),
				"FPS: %.1f | Update: %.3f ms | Draw: %.3f ms | Work: %.3f ms | ETA: %.1fs",
				(double)titleFrameCount / titleTimer,
				titleUpdateMsSum / titleSamples,
				titleDrawMsSum / titleSamples,
				stats.frameWorkMsSum / stats.frameWorkSamples,
				secondsRemaining
			);

			SDL_SetWindowTitle(app.window, title);

			titleTimer = 0.0;
			titleFrameCount = 0;
			titleUpdateMsSum = 0.0;
			titleDrawMsSum = 0.0;
			titleSamples = 0;
		}
	}

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
	stats.totalFrames = 0;

	stats.minFrameWorkMs = DBL_MAX;
	stats.maxFrameWorkMs = 0.0;
	stats.frameWorkMsSum = 0.0;
	stats.frameWorkSamples = 0;

	/////////////////////////

	while (1)
	{
		Uint64 frameStart = SDL_GetPerformanceCounter();

		double deltaTime =
			(double)(frameStart - lastCounter) /
			SDL_GetPerformanceFrequency();

		real deltaTimeReal = (real)deltaTime;

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
			deltaTimeReal
		);

		Uint64 updateEnd = SDL_GetPerformanceCounter();

		DrawBoids(boids, boidCount);

		for (size_t i = 0; i < poiCount; i++)
		{
			if (!pointsOfInterest[i].active)
			{
				pointsOfInterest[i] = poi_reinitialize(&pointsOfInterest[i]);
			}

			//poipoi_draw(&pointsOfInterest[i], poiColor);
		}

		draw_circle((int)boids[0].x, (int)boids[0].y, sim.visionRadius, (Color) { 255, 0, 255, 255 }, false);

		draw_circle((int)boids[0].x, (int)boids[0].y, sim.protectedRange, (Color) {255, 0, 255, 255}, false);

		Uint64 drawEnd = SDL_GetPerformanceCounter();

		presentScene();

		Uint64 frameEnd = SDL_GetPerformanceCounter();

		Uint64 performanceFreq = SDL_GetPerformanceFrequency();


		UpdateStats(deltaTime, updateStart, updateEnd, drawEnd, frameStart, frameEnd, performanceFreq);


		double frameSeconds =
			(double)(frameEnd - frameStart) / performanceFreq;

		double targetFrameTime = 1.0 / 60.0;

		stats.totalFrames++;

		if (BENCHMARK_FRAMES > 0 && stats.totalFrames >= BENCHMARK_FRAMES)
		{
			exit(0);
		}

		if (frameSeconds < targetFrameTime)
		{
			SDL_Delay((Uint32)((targetFrameTime - frameSeconds) * 1000.0));
		}
	}

	return 0;
}
