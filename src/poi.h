#pragma once
#include "boid.h"
#include "vector2.h"

#define POI_STRENGTH 0.5
#define POI_HEALTH 100
#define POI_RADIUS 50.0
#define POI_CONSUME_RADIUS 10.0


typedef struct
{
    double x;
    double y;
    double radius;
    double strength;
    bool active;
    int health;
} PointOfInterest;


PointOfInterest poi_create_random(void);
PointOfInterest poi_reinitialize(PointOfInterest* poi);
void poi_draw(PointOfInterest* poi);
vec2 poi_get_distance(PointOfInterest* poi, Boid* boid);
vec2 poi_get_force(PointOfInterest* poi, Boid* boid);
bool consume_poi(PointOfInterest* poi, Boid* boid, double consumeRadius, int damage);