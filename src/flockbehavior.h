#include "boid.h"
#include "poi.h"
#include "uniformgrid.h"

typedef struct
{
    int startIndex;
    int endIndex;

    Boid* current;
    Boid* next;

    UniformGrid* grid;

    SimulationParameters* sim;

    PointOfInterest* pois;
    int poiCount;

    real deltaTime;
} FlockJob;

FlockJob initFlockJob(int startIndex, int endIndex, const Boid* current, Boid* next, UniformGrid* grid, SimulationParameters* sim, PointOfInterest* pois, int poiCount, real deltaTime);

int WorkerMain(void* data);

void Flock(Boid* boid, Boid *boids, int numBoids, SimulationParameters* sim, PointOfInterest* pointsOfInterest, int poiCount, real deltaTime);

vec2 AvoidBorders(Boid* boid, real borderMargin);

void DrawBoids(Boid* boids, int numBoids, SimulationParameters* sim);
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

    int quit;
    int generation;
    int completed;
    int numThreads;

    FlockJob* jobs;
} WorkerPool;

WorkerPool pool;

int PersistentWorkerMain(void* data);