#include "poi.h"
#include "draw.h"

PointOfInterest poi_create_random(void)
{
	PointOfInterest poi;
	poi.x = rand_range_double(50.0, SCREEN_WIDTH - 50.0);
	poi.y = rand_range_double(50.0, SCREEN_HEIGHT - 50.0);
	poi.attractionRadius = POI_ATTRACTION_RADIUS;
	poi.attractionRadiusSq = POI_ATTRACTION_RADIUS * POI_ATTRACTION_RADIUS;
	poi.radius = POI_CONSUME_RADIUS;
	poi.radiusSq = POI_CONSUME_RADIUS * POI_CONSUME_RADIUS;
	poi.health = POI_HEALTH;
	poi.active = true;
	return poi;
}

PointOfInterest poi_reinitialize(PointOfInterest* poi)
{
	poi->x = rand_range_double(50.0, SCREEN_WIDTH - 50.0);
	poi->y = rand_range_double(50.0, SCREEN_HEIGHT - 50.0);
	poi->attractionRadius = POI_ATTRACTION_RADIUS;
	poi->attractionRadiusSq = POI_ATTRACTION_RADIUS * POI_ATTRACTION_RADIUS;
	poi->radius = POI_CONSUME_RADIUS;
	poi->radiusSq = POI_CONSUME_RADIUS * POI_CONSUME_RADIUS;
	poi->health = POI_HEALTH;
	poi->active = true;
	return *poi;
}

void poi_draw(PointOfInterest* poi, Color color)
{
	if (!poi->active)
	{
		return;
	}
	draw_circle(poi->x, poi->y, poi->radius, color, false);
	//draw_circle(poi->x, poi->y, poi->attractionRadius, color, false);
}

vec2 poi_get_direction_vector(PointOfInterest* poi, Boid* boid)
{
	vec2 boidPos = { boid->x, boid->y };
	vec2 poiPos = { poi->x, poi->y };
	return vec_sub(poiPos, boidPos);
}


vec2 poi_get_force(PointOfInterest* poi, Boid* boid, SimulationParameters* sim)
{
	if (!poi->active)
	{
		return (vec2) { 0, 0 };
	}
	vec2 boidPos = { boid->x, boid->y };
	vec2 poiPos = { poi->x, poi->y };
	vec2 direction = vec_sub(poiPos, boidPos);
	double distanceSq = vec_mag_sq(direction);
	if (distanceSq < poi->attractionRadiusSq)
	{
		double force = (1.0 - distanceSq / poi->attractionRadiusSq) * sim->poiFactor;
		return vec_mul(direction, force);
	}
	return (vec2){0, 0};
}

bool consume_poi(PointOfInterest* poi, Boid* boid, int damage)
{
	if (!poi->active)
	{
		return false;
	}
	vec2 boidPos = { boid->x, boid->y };
	vec2 poiPos = { poi->x, poi->y };
	double distSq = vec_mag_sq(vec_sub(poiPos, boidPos));
	if (distSq <= poi->radiusSq)
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
