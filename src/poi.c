#include "poi.h"
#include "draw.h"

PointOfInterest poi_create_random(void)
{
	PointOfInterest poi;
	poi.x = rand_range_real(BORDER_MARGIN, WORLD_WIDTH - BORDER_MARGIN);
	poi.y = rand_range_real(BORDER_MARGIN, WORLD_HEIGHT - BORDER_MARGIN);
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
	poi->x = rand_range_real(BORDER_MARGIN, WORLD_WIDTH - BORDER_MARGIN);
	poi->y = rand_range_real(BORDER_MARGIN, WORLD_HEIGHT - BORDER_MARGIN);
	poi->attractionRadius = POI_ATTRACTION_RADIUS;
	poi->attractionRadiusSq = POI_ATTRACTION_RADIUS * POI_ATTRACTION_RADIUS;
	poi->radius = POI_CONSUME_RADIUS;
	poi->radiusSq = POI_CONSUME_RADIUS * POI_CONSUME_RADIUS;
	poi->health = POI_HEALTH;
	poi->active = true;
	return *poi;
}

void poi_draw(Camera* camera, PointOfInterest* poi, DrawColor color)
{
	if (!poi->active)
	{
		return;
	}
	color.a = (Uint8)(color.a * ((real)poi->health / (real)POI_HEALTH));

	vec2 screenPos = WorldToScreen(camera, (vec2) { poi->x, poi->y });
	draw_circle(screenPos.x, screenPos.y, poi->radius * camera->zoom, color, false);
	//draw_circle(screenPos.x, screenPos.y, poi->attractionRadius * camera->zoom, color, false);
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
	real distanceSq = vec_mag_sq(direction);
	if (distanceSq < poi->attractionRadiusSq)
	{
		real force = (R(1.0) - distanceSq / poi->attractionRadiusSq) * R(sim->poiFactor);
		return vec_mul(vec_norm(direction), force);
	}
	return (vec2){0, 0};
}

vec2 poi_get_force_soa(PointOfInterest* poi, BoidSOA* boid, int boidIndex, SOASimulationParameters* sim)
{
	if (!poi->active)
	{
		return (vec2) { 0, 0 };
	}
	vec2 boidPos = { boid->x[boidIndex], boid->y[boidIndex] };
	vec2 poiPos = { poi->x, poi->y };
	vec2 direction = vec_sub(poiPos, boidPos);
	real distanceSq = vec_mag_sq(direction);
	if (distanceSq < poi->attractionRadiusSq)
	{
		real force = (R(1.0) - distanceSq / poi->attractionRadiusSq) * R(sim->poiFactor);
		return vec_mul(vec_norm(direction), force);
	}
	return (vec2) { 0, 0 };
}

bool consume_poi(PointOfInterest* poi, Boid* boid, int damage)
{
	if (!poi->active)
	{
		return false;
	}
	vec2 boidPos = { boid->x, boid->y };
	vec2 poiPos = { poi->x, poi->y };
	real distSq = vec_mag_sq(vec_sub(poiPos, boidPos));
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

bool consume_poi_soa(PointOfInterest* poi, BoidSOA* boid, int boidIndex, int damage)
{
	if (!poi->active)
	{
		return false;
	}
	vec2 boidPos = { boid->x[boidIndex], boid->y[boidIndex] };
	vec2 poiPos = { poi->x, poi->y };
	real distSq = vec_mag_sq(vec_sub(poiPos, boidPos));
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