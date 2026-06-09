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
#include "workerpool.h"
#include "stats.h"
#include "uniformgrid.h"


App    app;
Stats stats;
Camera camera;


void printStats(void);
vec2 GetCameraInputDirection();
void DrawBorders();
void UpdateStats(
	double deltaTime,
	bool gridBuiltThisFrame,
	Uint64 gridMemsetStart,
	Uint64 gridMemsetEnd,
	Uint64 gridCountStart,
	Uint64 gridCountEnd,
	Uint64 gridReduceStart,
	Uint64 gridReduceEnd,
	Uint64 gridPrefixStart,
	Uint64 gridPrefixEnd,
	Uint64 gridPrepareStart,
	Uint64 gridPrepareEnd,
	Uint64 gridBuildStart,
	Uint64 gridBuildEnd,
	Uint64 updateStart,
	Uint64 updateEnd,
	Uint64 drawStart,
	Uint64 drawEnd,
	Uint64 frameStart,
	Uint64 frameEnd,
	Uint64 performanceFreq);
void DrawBoidsBatchedPoints(Camera* camera, Boid* boids, int numBoids, SimulationParameters* sim, bool rebuildPoints);
void HandleObstacleInput(Obstacles** obstacles);
void RunFlockWorkers(WorkerPool* workerpool, UniformGrid* grid, int boidCount, Boid** boids, Boid** boidsNext, Obstacles* obstacles, SimulationParameters* sim, PointOfInterest* pointsOfInterest, int poiCount, real deltaTime);

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

	if (!UniformGrid_Init(&grid, R(WORLD_WIDTH), R(WORLD_HEIGHT), sim.visionRadius * R(2.0), boidCount)) {
		SDL_Log("Failed to initialize uniform grid");
		exit(1);
	}

	Obstacles* obstacles = NULL;

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
	Stats_Init(&stats);

	/*stats.minFps = DBL_MAX;
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
	stats.frameWorkSamples = 0;*/

	/////////////////////////
	int numThreads = SDL_GetCPUCount();
	int countThreads = numThreads;
	SDL_Thread** threads = malloc(sizeof(SDL_Thread*) * numThreads);
	char (*threadNames)[32] = malloc(sizeof(*threadNames) * numThreads);

	for (int i = 0; i < numThreads; i++)
	{
		snprintf(threadNames[i], sizeof(threadNames[i]),
			"BoidWorker %d", i);
	}


	// init workerpool
	workerpool.numThreads = numThreads;
	workerpool.quit = 0;
	workerpool.generation = 0;
	workerpool.completed = 0;
	workerpool.mutex = SDL_CreateMutex();
	workerpool.chunkMutex = SDL_CreateMutex();
	workerpool.startCond = SDL_CreateCond();
	workerpool.doneCond = SDL_CreateCond();

	int* threadIds = malloc(sizeof(int) * numThreads);
	if (threadIds == NULL)
	{
		SDL_Log("Failed to allocate threadIds");
		exit(1);
	}

	for (int i = 0; i < numThreads; i++)
	{
		threadIds[i] = i;
		threads[i] = SDL_CreateThread(
			PersistentWorkerMainBalanced,
			threadNames[i],
			&threadIds[i]
		);
	}


	GridCountJobData gridCountJobData = {
		.grid = &grid,
		.boids = boids,
		.cellCount = grid.cellCount
	};

	gridCountJobData.localCounts =
		malloc(sizeof(int) * countThreads * grid.cellCount);

	gridCountJobData.touchedCells =
		malloc(sizeof(int) * countThreads * boidCount);

	gridCountJobData.touchedCounts =
		malloc(sizeof(int) * countThreads);

	gridCountJobData.cellCount = grid.cellCount;
	gridCountJobData.maxTouchedPerThread = boidCount;

	if (gridCountJobData.localCounts == NULL || gridCountJobData.touchedCells == NULL || gridCountJobData.touchedCounts == NULL) {
		SDL_Log("Failed to allocate local counts for grid job data");
		exit(1);
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
		bool gridBuiltThisFrame = false;

		Uint64 gridMemsetStart = 0;
		Uint64 gridMemsetEnd = 0;
		Uint64 gridReduceStart = 0;
		Uint64 gridReduceEnd = 0;
		Uint64 gridCountStart = 0;
		Uint64 gridCountEnd = 0;
		Uint64 gridPrefixStart = 0;
		Uint64 gridPrefixEnd = 0;
		Uint64 gridPrepareStart = 0;
		Uint64 gridPrepareEnd = 0;
		Uint64 gridBuildStart = 0;
		Uint64 gridBuildEnd = 0;
		if (frameCount % 16 == 0)
		{
			gridBuiltThisFrame = true;

			gridMemsetStart = SDL_GetPerformanceCounter();

			// No more clearing numThreads * cellCount.
			// Only reset one int per thread.
			memset(
				gridCountJobData.touchedCounts,
				0,
				sizeof(int) * countThreads
			);

			gridMemsetEnd = SDL_GetPerformanceCounter();

			gridCountStart = gridMemsetEnd;

			WorkerPool_Run(
				&workerpool,
				boidCount,
				31250,
				UniformGrid_RunGridCountJob,
				&gridCountJobData
			);

			gridCountEnd = SDL_GetPerformanceCounter();

			gridReduceStart = gridCountEnd;

			UniformGrid_GridCountReduce(&grid, &gridCountJobData, countThreads);

			gridReduceEnd = SDL_GetPerformanceCounter();

			gridPrefixStart = gridReduceEnd;
			UniformGrid_PrefixSum(&grid);
			gridPrefixEnd = SDL_GetPerformanceCounter();
			//UniformGrid_PrepareBuild(&grid);
			gridPrepareStart = gridPrefixEnd;
			if (!UniformGrid_PrepareBuild(&grid, boidCount))
			{
				return 0;
			}
			gridPrepareEnd = SDL_GetPerformanceCounter();
			gridBuildStart = gridPrepareEnd;
			//UniformGrid_Build(&grid, boids, boidCount);
			WorkerPool_Run(
				&workerpool,
				boidCount,
				31250,
				UniformGrid_RunBuildJob,
				&gridCountJobData
			);
			gridBuildEnd = SDL_GetPerformanceCounter();
		}



		RunFlockWorkers(
			&workerpool,
			&grid,
			boidCount,
			&boids,
			&boidsNext,
			obstacles,
			&sim,
			pointsOfInterest,
			poiCount,
			deltaTime
		);

		Uint64 updateEnd = SDL_GetPerformanceCounter();

		Uint64 drawStart = updateEnd;

		vec2 cameraInputDir = GetCameraInputDirection();
		camera.x += cameraInputDir.x * CAMERA_SPEED * R(deltaTime);
		camera.y += cameraInputDir.y * CAMERA_SPEED * R(deltaTime);

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


		//UpdateStats(deltaTime, updateStart, gridCountEnd, gridBuildEnd, updateEnd, drawEnd, frameStart, frameEnd, performanceFreq);
		UpdateStats(
			deltaTime,
			gridBuiltThisFrame,
			gridMemsetStart,
			gridMemsetEnd,
			gridCountStart,
			gridCountEnd,
			gridReduceStart,
			gridReduceEnd,
			gridPrefixStart,
			gridPrefixEnd,
			gridPrepareStart,
			gridPrepareEnd,
			gridBuildStart,
			gridBuildEnd,
			updateStart,
			updateEnd,
			drawStart,
			drawEnd,
			frameStart,
			frameEnd,
			performanceFreq);

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

	for (int i = 0; i < STAT_COUNT; i++)
	{
		StatMetric* metric = &stats.metrics[i];

		if (metric->samples == 0 || metric->name == NULL)
		{
			continue;
		}

		printf("%s min/avg/max: %.4f / %.4f / %.4f\n",
			metric->name,
			metric->min,
			metric->sum / metric->samples,
			metric->max);
	}

	printf("\n------------------------\n");

	cleanup();
}

void UpdateStats(
	double deltaTime,
	bool gridBuiltThisFrame,
	Uint64 gridMemsetStart,
	Uint64 gridMemsetEnd,
	Uint64 gridCountStart,
	Uint64 gridCountEnd,
	Uint64 gridReduceStart,
	Uint64 gridReduceEnd,
	Uint64 gridPrefixStart,
	Uint64 gridPrefixEnd,
	Uint64 gridPrepareStart,
	Uint64 gridPrepareEnd,
	Uint64 gridBuildStart,
	Uint64 gridBuildEnd,
	Uint64 updateStart,
	Uint64 updateEnd,
	Uint64 drawStart,
	Uint64 drawEnd,
	Uint64 frameStart,
	Uint64 frameEnd,
	Uint64 performanceFreq)
{
	static double titleTimer = 0.0;
	static int titleFrameCount = 0;

	static double titleFpsSum = 0.0;
	static double titleMemsetGridMsSum = 0.0;
	static double titleCountGridMsSum = 0.0;
	static double titleReduceGridMsSum = 0.0;
	static double titlePrefixGridMsSum = 0.0;
	static double titlePrepareGridMsSum = 0.0;
	static double titleBuildGridMsSum = 0.0;
	static double titleUpdateMsSum = 0.0;
	static double titleDrawMsSum = 0.0;
	static double titleWorkMsSum = 0.0;

	static int titleMemsetGridSamples = 0;
	static int titleCountGridSamples = 0;
	static int titleReduceGridSamples = 0;
	static int titlePrefixGridSamples = 0;
	static int titlePrepareGridSamples = 0;
	static int titleBuildGridSamples = 0;
	static int titleUpdateSamples = 0;
	static int titleDrawSamples = 0;
	static int titleWorkSamples = 0;

	stats.runTime += deltaTime;
	stats.totalFrames++;

	if (stats.totalFrames <= WARMUP_FRAMES)
	{
		return;
	}

	double fps = 0.0;

	if (deltaTime > 0.0)
	{
		fps = 1.0 / deltaTime;
		Stats_AddSample(&stats, STAT_FPS, fps);
	}

	if (gridBuiltThisFrame)
	{
		double gridMemsetMs = TicksToMs(gridMemsetStart, gridMemsetEnd, performanceFreq);
		double gridCountMs = TicksToMs(gridCountStart, gridCountEnd, performanceFreq);
		double gridReduceMs = TicksToMs(gridReduceStart, gridReduceEnd, performanceFreq);
		double gridPrefixMs = TicksToMs(gridPrefixStart, gridPrefixEnd, performanceFreq);
		double gridPrepareMs = TicksToMs(gridPrepareStart, gridPrepareEnd, performanceFreq);
		double gridBuildMs = TicksToMs(gridBuildStart, gridBuildEnd, performanceFreq);

		Stats_AddSample(&stats, STAT_GRID_MEMSET, gridMemsetMs);
		Stats_AddSample(&stats, STAT_GRID_COUNT, gridCountMs);
		Stats_AddSample(&stats, STAT_GRID_REDUCE, gridReduceMs);
		Stats_AddSample(&stats, STAT_GRID_PREFIX, gridPrefixMs);
		Stats_AddSample(&stats, STAT_GRID_PREPARE, gridPrepareMs);
		Stats_AddSample(&stats, STAT_GRID_BUILD, gridBuildMs);

		titleMemsetGridMsSum += gridMemsetMs;
		titleCountGridMsSum += gridCountMs;
		titleReduceGridMsSum += gridReduceMs;
		titlePrefixGridMsSum += gridPrefixMs;
		titlePrepareGridMsSum += gridPrepareMs;
		titleBuildGridMsSum += gridBuildMs;

		titleMemsetGridSamples++;
		titleCountGridSamples++;
		titleReduceGridSamples++;
		titlePrefixGridSamples++;
		titlePrepareGridSamples++;
		titleBuildGridSamples++;
	}

	double updateMs = TicksToMs(updateStart, updateEnd, performanceFreq);
	double drawMs = TicksToMs(drawStart, drawEnd, performanceFreq);
	double frameWorkMs = TicksToMs(frameStart, frameEnd, performanceFreq);

	Stats_AddSample(&stats, STAT_UPDATE, updateMs);
	Stats_AddSample(&stats, STAT_DRAW, drawMs);
	Stats_AddSample(&stats, STAT_FRAME_WORK, frameWorkMs);

	titleTimer += deltaTime;
	titleFrameCount++;

	titleFpsSum += fps;
	titleUpdateMsSum += updateMs;
	titleDrawMsSum += drawMs;
	titleWorkMsSum += frameWorkMs;

	titleUpdateSamples++;
	titleDrawSamples++;
	titleWorkSamples++;

	if (titleTimer >= 1.0)
	{
		int framesRemaining = BENCHMARK_FRAMES - stats.totalFrames;
		double secondsRemaining = framesRemaining / FRAME_RATE;

		double titleFps = titleFrameCount > 0 ? titleFpsSum / titleFrameCount : 0.0;
		double titleMemsetGridMs = titleMemsetGridSamples > 0 ? titleMemsetGridMsSum / titleMemsetGridSamples : 0.0;
		double titleCountGridMs = titleCountGridSamples > 0 ? titleCountGridMsSum / titleCountGridSamples : 0.0;
		double titleReduceGridMs = titleReduceGridSamples > 0 ? titleReduceGridMsSum / titleReduceGridSamples : 0.0;
		double titlePrefixGridMs = titlePrefixGridSamples > 0 ? titlePrefixGridMsSum / titlePrefixGridSamples : 0.0;
		double titlePrepareGridMs = titlePrepareGridSamples > 0 ? titlePrepareGridMsSum / titlePrepareGridSamples : 0.0;
		double titleBuildGridMs = titleBuildGridSamples > 0 ? titleBuildGridMsSum / titleBuildGridSamples : 0.0;
		double titleUpdateMs = titleUpdateSamples > 0 ? titleUpdateMsSum / titleUpdateSamples : 0.0;
		double titleDrawMs = titleDrawSamples > 0 ? titleDrawMsSum / titleDrawSamples : 0.0;
		double titleWorkMs = titleWorkSamples > 0 ? titleWorkMsSum / titleWorkSamples : 0.0;

		char title[TITLE_SIZE];

		snprintf(
			title,
			sizeof(title),
			"FPS: %.1f | Memset: %.3f ms | Count: %.3f ms | Reduce %.3f ms | Prefix: %.3f ms | Prepare: %.3f ms | Build: %.3f ms | Update: %.3f ms | Draw: %.3f ms | Work: %.3f ms | ETA: %.1fs",
			titleFps,
			titleMemsetGridMs,
			titleCountGridMs,
			titleReduceGridMs,
			titlePrefixGridMs,
			titlePrepareGridMs,
			titleBuildGridMs,
			titleUpdateMs,
			titleDrawMs,
			titleWorkMs,
			secondsRemaining
		);

		SDL_SetWindowTitle(app.window, title);

		titleTimer = 0.0;
		titleFrameCount = 0;

		titleFpsSum = 0.0;
		titleMemsetGridMsSum = 0.0;
		titleCountGridMsSum = 0.0;
		titleReduceGridMsSum = 0.0;
		titlePrefixGridMsSum = 0.0;
		titlePrepareGridMsSum = 0.0;
		titleBuildGridMsSum = 0.0;
		titleUpdateMsSum = 0.0;
		titleDrawMsSum = 0.0;
		titleWorkMsSum = 0.0;

		titleMemsetGridSamples = 0;
		titleCountGridSamples = 0;
		titleReduceGridSamples = 0;
		titlePrefixGridSamples = 0;
		titlePrepareGridSamples = 0;
		titleBuildGridSamples = 0;
		titleUpdateSamples = 0;
		titleDrawSamples = 0;
		titleWorkSamples = 0;
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


	draw_line(topLeft.x, topLeft.y, topRight.x, topRight.y, (DrawColor) { 255, 255, 255, 255 });
	draw_line(topRight.x, topRight.y, bottomRight.x, bottomRight.y, (DrawColor) { 255, 255, 255, 255 });
	draw_line(bottomRight.x, bottomRight.y, bottomLeft.x, bottomLeft.y, (DrawColor) { 255, 255, 255, 255 });
	draw_line(bottomLeft.x, bottomLeft.y, topLeft.x, topLeft.y, (DrawColor) { 255, 255, 255, 255 });
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


void RunFlockWorkers(
	WorkerPool* workerpool,
	UniformGrid* grid,
	int boidCount,
	Boid** boids,
	Boid** boidsNext,
	Obstacles* obstacles,
	SimulationParameters* sim,
	PointOfInterest* pointsOfInterest,
	int poiCount,
	real deltaTime
)
{
	FlockJob job = initFlockJob(
		0,
		boidCount,
		*boids,
		*boidsNext,
		grid,
		obstacles,
		sim,
		pointsOfInterest,
		poiCount,
		deltaTime
	);

	WorkerPool_Run(
		workerpool,
		boidCount,
		256,
		FlockJob_Run,
		&job
	);

	Boid* tmp = *boids;
	*boids = *boidsNext;
	*boidsNext = tmp;
}