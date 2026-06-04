#include "boid.h"
#include "poi.h"
#include "uniformgrid.h"
#include "obstacles.h"
#include "camera.h"

typedef struct
{
    int startIndex;
    int endIndex;

    Boid* current;
    Boid* next;

    UniformGrid* grid;

	Obstacles* obstacles;

    SimulationParameters* sim;

    PointOfInterest* pois;
    int poiCount;

    real deltaTime;
} FlockJob;

FlockJob initFlockJob(int startIndex, int endIndex, const Boid* current, Boid* next, UniformGrid* grid, Obstacles* obstacles, SimulationParameters* sim, PointOfInterest* pois, int poiCount, real deltaTime);

int WorkerMain(void* data);

void Flock(Boid* boid, Boid *boids, int numBoids, SimulationParameters* sim, PointOfInterest* pointsOfInterest, int poiCount, real deltaTime);

vec2 AvoidBorders(Boid* boid, real borderMargin);

vec2 AvoidObstacle(Boid* boid, Obstacles* obstacle, real avoidDistance);

void DrawBoids(Camera* camera, Boid* boids, int numBoids, SimulationParameters* sim);
void UpdateBoids(Boid* boids, int numBoids, SimulationParameters* sim, PointOfInterest* pointsOfInterest, int poiCount, real deltaTime);

void UpdateBoidsGrid(
    Boid* boids,
    int numBoids,
    UniformGrid* grid,
    SimulationParameters* sim,
    PointOfInterest* pointsOfInterest,
    int poiCount,
    real deltaTime
);

void FlockGrid(
    int boidIndex,
    const Boid* current,
    Boid* boid,
    UniformGrid* grid,
    Obstacles* obstacles,
    SimulationParameters* sim,
    PointOfInterest* pointsOfInterest,
    int poiCount,
    real deltaTime
);


typedef struct
{
    SDL_mutex* mutex;
    SDL_cond* startCond;
    SDL_cond* doneCond;

    SDL_mutex* chunkMutex;

    int quit;
    int generation;
    int completed;
    int numThreads;

    int nextChunkStart;
    int chunkSize;
    int boidCount;

    FlockJob baseJob;

    FlockJob* jobs;
} WorkerPool;

WorkerPool pool;

int PersistentWorkerMain(void* data);
int PersistentWorkerMainBalanced(void* data);