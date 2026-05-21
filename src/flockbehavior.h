#include "boid.h"
#include "poi.h"
void Flock(Boid* boid, Boid *boids, int numBoids, SimulationParameters sim, PointOfInterest* pointsOfInterest, int poiCount, double deltaTime);

vec2 AvoidBorders(Boid* boid, double borderMargin);

void DrawBoids(Boid* boids, int numBoids);
void UpdateBoids(Boid* boids, int numBoids, SimulationParameters sim, PointOfInterest* pointsOfInterest, int poiCount, double deltaTime);