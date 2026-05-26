#include "boidSOA.h"
#include "poi.h"

void FlockSOA(BoidSOA* boidSOA, int boidIndex, SOASimulationParameters* sim, PointOfInterest* pointsOfInterest, int poiCount, real deltaTime);

vec2 AvoidBordersSOA(BoidSOA* boidSOA, int boidIndex, real borderMargin);

void DrawBoidsSOA(BoidSOA* boids, SOASimulationParameters* sim);
void UpdateBoidsSOA(BoidSOA* boids, SOASimulationParameters* sim, PointOfInterest* pointsOfInterest, int poiCount, real deltaTime);