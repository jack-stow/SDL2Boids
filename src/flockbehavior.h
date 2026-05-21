#include "boid.h"
#include "poi.h"
void Flock(Boid* boid, Boid *boids, int numBoids, SimulationParameters sim, PointOfInterest* pointsOfInterest, int poiCount, real deltaTime);

vec2 AvoidBorders(Boid* boid, real borderMargin);

void DrawBoids(Boid* boids, int numBoids);
void UpdateBoids(Boid* boids, int numBoids, SimulationParameters sim, PointOfInterest* pointsOfInterest, int poiCount, real deltaTime);