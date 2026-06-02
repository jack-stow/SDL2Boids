#pragma once
#include "boid.h"
#include "vector2.h"
#include "boidSOA.h"

#define NUM_POI 10
#define POI_HEALTH 500
#define POI_ATTRACTION_RADIUS 500.0
#define POI_CONSUME_RADIUS 30.0


typedef struct
{
    real x;
    real y;
    real attractionRadius;
    real attractionRadiusSq;
    real radius;
    real radiusSq;
    bool active;
    int health;
} PointOfInterest;


PointOfInterest poi_create_random(void);
PointOfInterest poi_reinitialize(PointOfInterest* poi);
void poi_draw(PointOfInterest* poi, DrawColor color);
vec2 poi_get_direction_vector(PointOfInterest* poi, Boid* boid);
vec2 poi_get_force(PointOfInterest* poi, Boid* boid, SimulationParameters* sim);
vec2 poi_get_force_soa(PointOfInterest* poi, BoidSOA* boid, int boidIndex, SOASimulationParameters* sim);
bool consume_poi(PointOfInterest* poi, Boid* boid, int damage);
bool consume_poi_soa(PointOfInterest* poi, BoidSOA* boid, int boidIndex, int damage);