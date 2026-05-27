#include "boid.h"
#include "poi.h"
#include "uniformgrid.h"

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
    Boid* boids,
    UniformGrid* grid,
    SimulationParameters* sim,
    PointOfInterest* pointsOfInterest,
    int poiCount,
    real deltaTime
);