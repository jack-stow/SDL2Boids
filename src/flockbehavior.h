#include "boid.h"
void Flock(Boid* boid, Boid *boids, int numBoids, double avoidFactor, double matchingFactor, double centeringFactor, int maxVisible, double visionRadius, double protectedRange, double deltaTime);

vec2 AvoidBorders(Boid* boid, double borderMargin);