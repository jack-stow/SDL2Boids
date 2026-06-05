#include "common.h"

#include "draw.h"
#include "init.h"
#include "input.h"
#include "main.h"
#include "boid.h"
#include "flockbehavior.h"
#include "poi.h"
#include "obstacles.h"
#include "camera.h"


App    app;
Stats stats;
Camera camera;


void displayFPS(double fps)
{
	char title[TITLE_SIZE];
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
	
	printf("Build Grid ms min/avg/max: %.4f / %.4f / %.4f\n",
		stats.minGridBuildMs,
		stats.gridBuildMsSum / stats.gridBuildSamples,
		stats.maxGridBuildMs);

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

void UpdateStats(double deltaTime, Uint64 updateStart, Uint64 gridBuildEnd, Uint64 updateEnd, Uint64 drawEnd, Uint64 frameStart, Uint64 frameEnd, Uint64 performanceFreq) {

	static double titleTimer = 0.0;
	static int titleFrameCount = 0;
	static double titleBuildGridMsSum = 0.0;
	static double titleUpdateMsSum = 0.0;
	static double titleDrawMsSum = 0.0;
	static int titleSamples = 0;

	double gridBuildMs =
		(double)(gridBuildEnd - updateStart) / performanceFreq * MS_PER_SECOND;

	double updateMs =
		(double)(updateEnd - gridBuildEnd) / performanceFreq * MS_PER_SECOND;

	double drawMs =
		(double)(drawEnd - updateEnd) / performanceFreq * MS_PER_SECOND;

	double presentMs =
		(double)(frameEnd - drawEnd) / performanceFreq * MS_PER_SECOND;

	double frameWorkMs =
		(double)(frameEnd - frameStart) / performanceFreq * MS_PER_SECOND;


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


		if (gridBuildMs < stats.minGridBuildMs) stats.minGridBuildMs = gridBuildMs;
		if (gridBuildMs > stats.maxGridBuildMs) stats.maxGridBuildMs = gridBuildMs;
		stats.gridBuildMsSum += gridBuildMs;
		stats.gridBuildSamples++;

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

		titleBuildGridMsSum += gridBuildMs;
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
		double secondsRemaining = framesRemaining / FRAME_RATE;

		if (titleTimer >= 1.0)
		{
			char title[TITLE_SIZE];

			snprintf(
				title,
				sizeof(title),
				"FPS: %.1f | Build Grid: %.3f ms | Update: %.3f ms | Draw: %.3f ms | Work: %.3f ms | ETA: %.1fs",
				(double)titleFrameCount / titleTimer,
				titleBuildGridMsSum / titleSamples,
				titleUpdateMsSum / titleSamples,
				titleDrawMsSum / titleSamples,
				stats.frameWorkMsSum / stats.frameWorkSamples,
				secondsRemaining
			);

			SDL_SetWindowTitle(app.window, title);

			titleTimer = 0.0;
			titleFrameCount = 0;
			titleBuildGridMsSum = 0.0;
			titleUpdateMsSum = 0.0;
			titleDrawMsSum = 0.0;
			titleSamples = 0;
		}
	}

}

void DrawBoidsBatchedPoints(Camera* camera, Boid* boids, int numBoids, SimulationParameters* sim, bool rebuildPoints)
{
	static SDL_Point* points = NULL;
	static int capacity = 0;
	static int pointCount = 0;

	if (numBoids > capacity)
	{
		SDL_Point* newPoints = realloc(points, sizeof(SDL_Point) * numBoids);

		if (newPoints == NULL)
		{
			SDL_Log("Failed to allocate point batch");
			return;
		}

		points = newPoints;
		capacity = numBoids;
	}

	if (rebuildPoints)
	{
		for (int i = 0; i < numBoids; i++)
		{
			vec2 screen = WorldToScreen(camera, (vec2) { boids[i].x, boids[i].y });

			points[i].x = (int)screen.x;
			points[i].y = (int)screen.y;
		}

		pointCount = numBoids;
	}

	SDL_SetRenderDrawColor(
		app.renderer,
		sim->boidColor.r,
		sim->boidColor.g,
		sim->boidColor.b,
		sim->boidColor.a
	);

	SDL_RenderDrawPoints(app.renderer, points, pointCount);
}

vec2 GetCameraInputDirection()
{
	vec2 dir = { 0, 0 };
	if (app.up) dir.y -= 1;
	if (app.down) dir.y += 1;
	if (app.left) dir.x -= 1;
	if (app.right) dir.x += 1;
	return vec_norm(dir);
}

void DrawBorders() {

	vec2 topLeft = WorldToScreen(&camera, (vec2) { 0, 0 });
	vec2 topRight = WorldToScreen(&camera, (vec2) { WORLD_WIDTH, 0 });
	vec2 bottomLeft = WorldToScreen(&camera, (vec2) { 0, WORLD_HEIGHT });
	vec2 bottomRight = WorldToScreen(&camera, (vec2) { WORLD_WIDTH, WORLD_HEIGHT });


	draw_line(topLeft.x, topLeft.y, topRight.x, topRight.y, (DrawColor) {255, 255, 255, 255});
	draw_line(topRight.x, topRight.y, bottomRight.x, bottomRight.y, (DrawColor) {255, 255, 255, 255});
	draw_line(bottomRight.x, bottomRight.y, bottomLeft.x, bottomLeft.y, (DrawColor) {255, 255, 255, 255});
	draw_line(bottomLeft.x, bottomLeft.y, topLeft.x, topLeft.y, (DrawColor) {255, 255, 255, 255});
}

void HandleObstacleInput(Obstacles** obstacles)
{
	static bool drawingLine = false;
	vec2 mouseWorldPos = ScreenToWorld(&camera, (vec2) { app.mouseX, app.mouseY });
	vec2 dragStartWorldPos = ScreenToWorld(&camera, (vec2) { app.dragStartX, app.dragStartY });
	vec2 dragEndWorldPos = ScreenToWorld(&camera, (vec2) { app.dragEndX, app.dragEndY });
	vec2 rMouseWorldPos = ScreenToWorld(&camera, (vec2) { app.rmouseX, app.rmouseY });
	vec2 prevRMouseWorldPos = ScreenToWorld(&camera, (vec2) { app.prevRMouseX, app.prevRMouseY });

	if (app.mouseDown)
	{
		draw_line(app.dragStartX, app.dragStartY,
			app.mouseX, app.mouseY,
			(DrawColor) {
			255, 255, 255, 255
		});

		drawingLine = true;
	}
	else if (drawingLine)
	{
		Obstacles_Add(obstacles,
			(real)dragStartWorldPos.x,
			(real)dragStartWorldPos.y,
			(real)dragEndWorldPos.x,
			(real)dragEndWorldPos.y);
		drawingLine = false;
	}

	if (app.rmouseDown)
	{
		draw_line(app.prevRMouseX, app.prevRMouseY,
			app.rmouseX, app.rmouseY,
			(DrawColor) {
			255, 0, 0, 255
		});

		Obstacles_EraseIntersecting(obstacles,
			prevRMouseWorldPos.x,
			prevRMouseWorldPos.y,
			rMouseWorldPos.x,
			rMouseWorldPos.y);
	}
}

int main(int argc, char* argv[])
{
	memset(&app, 0, sizeof(App));
	srand(RNG_SEED);
	initSDL();

	//atexit(cleanup);
	atexit(printStats);
	real posX = 100.0;
	real posY = 100.0;

	camera.x = 0;
	camera.y = 0;
	camera.zoom = 1.0;
	camera.minZoom = 0.01f;
	camera.maxZoom = 50.0f;
	camera.screenW = SCREEN_WIDTH;
	camera.screenH = SCREEN_HEIGHT;

	SimulationParameters sim = {

		.topSpeed = R(TOP_SPEED),
		.minSpeed = R(MIN_SPEED),
		.turnSpeed = R(TURN_SPEED),
		.acceleration = R(ACCELERATION),

		.scale = R(BOID_SCALE),

		.avoidFactor = R(AVOID_FACTOR),
		.matchingFactor = R(MATCHING_FACTOR),
		.centeringFactor = R(CENTERING_FACTOR),
		.borderingFactor = R(BORDERING_FACTOR),
		.obstacleAvoidFactor = R(OBSTACLE_AVOID_FACTOR),

		.maxVisible = MAX_VISIBLE,
		.visionRadius = R(VISION_RADIUS),
		.visionRadiusSq = R(VISION_RADIUS) * R(VISION_RADIUS),
		.protectedRange = R(PROTECTED_RANGE),
		.protectedRangeSq = R(PROTECTED_RANGE) * R(PROTECTED_RANGE),

		.poiFactor = R(POI_FACTOR),

		.obstacleAvoidDistance = R(OBSTACLE_AVOID_DISTANCE),

		.texture = loadTexture(BOID_TEXTURE),
		.boidColor = (DrawColor){255, 0, 0, 255}
	};

	int boidCount = BOID_COUNT;

	Boid* boids = malloc(sizeof(Boid) * boidCount);

	Boid* boidsNext = malloc(sizeof(Boid) * boidCount);

	if (boids == NULL || boidsNext == NULL)
	{
		SDL_Log("Failed to allocate boids");
		exit(1);
	}

	for (size_t i = 0; i < boidCount; i++)
	{
		boids[i] = boid_create(&sim);
	}

	//BoidSOA* boids = boidsoa_create(&sim);

	UniformGrid grid;

	if (!UniformGrid_Init(
		&grid,
		R(WORLD_WIDTH),
		R(WORLD_HEIGHT),
		sim.visionRadius * R(2.0),
		boidCount
	)) {
		SDL_Log("Failed to initialize uniform grid");
		exit(1);
	}

	Obstacles* obstacles = NULL;// malloc(sizeof(Obstacles));



	Obstacles* nextObstacle = obstacles;

	int poiCount = NUM_POI;
	PointOfInterest* pointsOfInterest = malloc(sizeof(PointOfInterest) * poiCount);

	if (pointsOfInterest == NULL)
	{
		SDL_Log("Failed to allocate points of interest");
		exit(1);
	}
	for (size_t i = 0; i < poiCount; i++)
	{
		pointsOfInterest[i] = poi_create_random();
	}

	Uint64 lastCounter = SDL_GetPerformanceCounter();

	double fpsTimer = 0.0;
	Uint64 frameCount = 0;

	Uint64 updateStart = 0;
	Uint64 updateEnd = 0;
	Uint64 drawEnd = 0;

	double flockTimeAccum = 0.0;
	int flockCallCount = 0;
	double statsTimer = 0.0;

	DrawColor poiColor = { 0, 255, 0, 255 };


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
	int numThreads = SDL_GetCPUCount();
	SDL_Thread** threads = malloc(sizeof(SDL_Thread*) * numThreads);
	char (*threadNames)[32] = malloc(sizeof(*threadNames) * numThreads);
	FlockJob* jobs = malloc(sizeof(FlockJob) * numThreads);

	for (int i = 0; i < numThreads; i++)
	{
		snprintf(threadNames[i], sizeof(threadNames[i]),
			"BoidWorker %d", i);
	}


	pool.numThreads = numThreads;
	pool.jobs = jobs;
	pool.quit = 0;
	pool.generation = 0;
	pool.completed = 0;
	pool.mutex = SDL_CreateMutex();
	pool.chunkMutex = SDL_CreateMutex();
	pool.startCond = SDL_CreateCond();
	pool.doneCond = SDL_CreateCond();

	int* threadIds = malloc(sizeof(int) * numThreads);

	for (int i = 0; i < numThreads; i++)
	{
		threadIds[i] = i;
		threads[i] = SDL_CreateThread(
			PersistentWorkerMainBalanced,
			threadNames[i],
			&threadIds[i]
		);
	}


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

		if (app.mouseWheelY != 0)
		{
			real zoomStep = 1.01f;
			real zoomFactor = powf(zoomStep, app.mouseWheelY);

			Camera_ZoomAt(
				&camera,
				(vec2) {
				app.mouseX, app.mouseY
			},
				zoomFactor
			);
		}

		Uint64 updateStart = SDL_GetPerformanceCounter();

		/*UpdateBoidsSOA(boids, &sim, pointsOfInterest, poiCount, deltaTimeReal);*/

		//UpdateBoids(boids, boidCount, &sim, pointsOfInterest, poiCount, deltaTimeReal);
		if (frameCount % 16 == 0) {
			UniformGrid_Build(&grid, boids, boidCount);
		}

		Uint64 gridBuildEnd = SDL_GetPerformanceCounter();


		/*for (int i = 0; i < numThreads; i++)
		{
			int start = i * boidCount / numThreads;
			int end = (i + 1) * boidCount / numThreads;

			jobs[i] = initFlockJob(
				start,
				end,
				boids,
				boidsNext,
				&grid,
				obstacles,
				&sim,
				pointsOfInterest,
				poiCount,
				deltaTimeReal
			);

		}*/

		pool.nextChunkStart = 0;
		pool.chunkSize = 256;   // tune: 256, 512, 1024, 2048
		pool.boidCount = boidCount;

		pool.baseJob = initFlockJob(
			0,
			boidCount,
			boids,
			boidsNext,
			&grid,
			obstacles,
			&sim,
			pointsOfInterest,
			poiCount,
			deltaTimeReal
		);

		SDL_LockMutex(pool.mutex);
		pool.completed = 0;
		pool.generation++;
		SDL_CondBroadcast(pool.startCond);

		while (pool.completed < numThreads)
		{
			SDL_CondWait(pool.doneCond, pool.mutex);
		}

		SDL_UnlockMutex(pool.mutex);

		Boid* tmp = boids;
		boids = boidsNext;
		boidsNext = tmp;


		//camera.x = boids[0].x - (camera.screenW / (2.0f * camera.zoom));
		//camera.y = boids[0].y - (camera.screenH / (2.0f * camera.zoom));

		//UpdateBoidsGrid(boids, boidCount, &grid, &sim, pointsOfInterest, poiCount, deltaTimeReal);

		Uint64 updateEnd = SDL_GetPerformanceCounter();


		vec2 cameraInputDir = GetCameraInputDirection();
		camera.x += cameraInputDir.x * CAMERA_SPEED * deltaTime;
		camera.y += cameraInputDir.y * CAMERA_SPEED * deltaTime;

		//DrawBoids(boids, boidCount, &sim);
		bool rebuildDrawPoints = frameCount % 2 == 0;
		DrawBoidsBatchedPoints(&camera, boids, boidCount, &sim, rebuildDrawPoints);
		
		for (size_t i = 0; i < poiCount; i++)
		{
			if (!pointsOfInterest[i].active)
			{
				pointsOfInterest[i] = poi_reinitialize(&pointsOfInterest[i]);
			}

			//poi_draw(&camera, &pointsOfInterest[i], poiColor);
		}

		vec2 screenPos = WorldToScreen(&camera, (vec2) { boids[0].x, boids[0].y });

		/*draw_circle(screenPos.x, screenPos.y, sim.visionRadius * camera.zoom, (DrawColor) { 255, 0, 255, 255 }, false);

		draw_circle(screenPos.x, screenPos.y, sim.protectedRange * camera.zoom, (DrawColor) {255, 0, 255, 255}, false);*/

		HandleObstacleInput(&obstacles);


		// Draw line while dragging left mouse, create obstacle on release
		DrawBorders();
		Obstacles_Draw(&camera, obstacles);

		Uint64 drawEnd = SDL_GetPerformanceCounter();

		presentScene();

		Uint64 frameEnd = SDL_GetPerformanceCounter();

		Uint64 performanceFreq = SDL_GetPerformanceFrequency();


		UpdateStats(deltaTime, updateStart, gridBuildEnd, updateEnd, drawEnd, frameStart, frameEnd, performanceFreq);


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
			SDL_Delay((Uint32)((targetFrameTime - frameSeconds) * MS_PER_SECOND));
		}
		frameCount++;
	}

	return 0;
}
