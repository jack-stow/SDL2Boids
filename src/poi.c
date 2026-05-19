#include "poi.h"
#include "draw.h"

PointOfInterest poi_create_random(void)
{
	PointOfInterest poi;
	poi.x = rand_range_double(50.0, SCREEN_WIDTH - 50.0);
	poi.y = rand_range_double(50.0, SCREEN_HEIGHT - 50.0);
	poi.radius = POI_RADIUS;
	poi.strength = rand_range_double(0.1, 1.0) * POI_STRENGTH;
	poi.health = POI_HEALTH;
	poi.active = true;
	return poi;
}

PointOfInterest poi_reinitialize(PointOfInterest* poi)
{
	poi->x = rand_range_double(50.0, SCREEN_WIDTH - 50.0);
	poi->y = rand_range_double(50.0, SCREEN_HEIGHT - 50.0);
	poi->radius = POI_RADIUS;
	poi->strength = rand_range_double(0.1, 1.0) * POI_STRENGTH;
	poi->health = POI_HEALTH;
	poi->active = true;
	return *poi;
}

void poi_draw(PointOfInterest* poi)
{
	if (!poi->active)
	{
		return;
	}
	draw_circle(poi->x, poi->y, 10.0);
}

vec2 poi_get_distance(PointOfInterest* poi, Boid* boid)
{
	vec2 boidPos = { boid->x, boid->y };
	vec2 poiPos = { poi->x, poi->y };
	return vec_sub(poiPos, boidPos);
}

vec2 poi_get_force(PointOfInterest* poi, Boid* boid)
{
	if (!poi->active)
	{
		return (vec2) { 0, 0 };
	}
	vec2 boidPos = { boid->x, boid->y };
	vec2 poiPos = { poi->x, poi->y };
	vec2 direction = vec_sub(poiPos, boidPos);
	double distance = vec_mag(direction);
	if (distance < poi->radius)
	{
		double force = (1.0 - distance / poi->radius) * poi->strength;
		return vec_mul(direction, force);
	}
	return (vec2){0, 0};
}

bool consume_poi(PointOfInterest* poi, Boid* boid, double consumeRadius, int damage)
{
	if (!poi->active)
	{
		return false;
	}
	vec2 boidPos = { boid->x, boid->y };
	vec2 poiPos = { poi->x, poi->y };
	double dist = vec_mag(vec_sub(poiPos, boidPos));
	if (dist <= consumeRadius)
	{
		poi->health -= damage;
		if (poi->health <= 0)
		{
			poi->active = false;
		}	
		return true;
	}
	return false;
}
