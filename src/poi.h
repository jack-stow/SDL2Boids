#pragma once
#include "boid.h"
#include "vector2.h"

#define POI_HEALTH 100
#define POI_ATTRACTION_RADIUS 200.0
#define POI_CONSUME_RADIUS 10.0


typedef struct
{
    double x;
    double y;
    double attractionRadius;
    double radius;
    bool active;
    int health;
} PointOfInterest;


PointOfInterest poi_create_random(void);
PointOfInterest poi_reinitialize(PointOfInterest* poi);
void poi_draw(PointOfInterest* poi, Color color);
vec2 poi_get_distance(PointOfInterest* poi, Boid* boid);
vec2 poi_get_force(PointOfInterest* poi, Boid* boid, SimulationParameters* sim);
bool consume_poi(PointOfInterest* poi, Boid* boid, int damage);