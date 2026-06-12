#include "flockbehavior.h"


void DrawBoids(Camera* camera, Boid* boids, int numBoids, SimulationParameters* sim) {
    set_draw_color((DrawColor) { 255, 0, 0, 255 });
	for (size_t i = 0; i < numBoids; i++)
	{
		drawBoid(camera, &boids[i], sim);
	}
}

void UpdateBoids(Boid* boids, int numBoids, SimulationParameters* sim, PointOfInterest* pointsOfInterest, int poiCount, real deltaTime) {
    
    for (size_t i = 0; i < numBoids; i++)
    {
        Flock(&boids[i], boids, numBoids, sim, pointsOfInterest, poiCount, deltaTime);

        boids[i].x += boids[i].speed.x * deltaTime;
        boids[i].y += boids[i].speed.y * deltaTime;


        if (boids[i].x < 0) boids[i].x += WORLD_WIDTH;
        if (boids[i].x > WORLD_WIDTH) boids[i].x -= WORLD_WIDTH;

        if (boids[i].y < 0) boids[i].y += WORLD_HEIGHT;
        if (boids[i].y > WORLD_HEIGHT) boids[i].y -= WORLD_HEIGHT;
    }
}

void UpdateBoidsGrid(Boid* boids, int numBoids, UniformGrid* grid, SimulationParameters* sim, PointOfInterest* pointsOfInterest, int poiCount, real deltaTime)
{
    if (!UniformGrid_Build(grid, boids, numBoids)) {
        return;
    }

    for (int i = 0; i < numBoids; i++)
    {
        //FlockGrid(i, boids, grid, sim, pointsOfInterest, poiCount, deltaTime);

        boids[i].x += boids[i].speed.x * deltaTime;
        boids[i].y += boids[i].speed.y * deltaTime;

        if (boids[i].x < 0) boids[i].x += WORLD_WIDTH;
        if (boids[i].x > WORLD_WIDTH) boids[i].x -= WORLD_WIDTH;

        if (boids[i].y < 0) boids[i].y += WORLD_HEIGHT;
        if (boids[i].y > WORLD_HEIGHT) boids[i].y -= WORLD_HEIGHT;
    }
}

void FlockGrid(
    int boidIndex,
    const Boid* current,
    Boid* boid,
    UniformGrid* grid,
	Obstacles* obstacles,
    SimulationParameters* sim,
    PointOfInterest* pointsOfInterest,
    int poiCount,
    real deltaTime
)
{
    vec2 avoid = { 0, 0 };
    vec2 align = { 0, 0 };
    vec2 cohere = { 0, 0 };

    int avoidCount = 0;
    int alignCount = 0;
    int cohesionCount = 0;

    vec2 boidPos = { boid->x, boid->y };

    GridQuery query = UniformGrid_GetQueryRange(
        grid,
        boid->x,
        boid->y,
        sim->visionRadius
    );

    for (int row = query.minRow; row <= query.maxRow; row++)
    {
        for (int col = query.minCol; col <= query.maxCol; col++)
        {
            const GridCell* cell = UniformGrid_GetCellConst(grid, col, row);

            if (cell == NULL)
                continue;

            for (int n = 0; n < cell->count; n++)
            {
                int otherIndex = grid->indices[cell->start + n];

                if (otherIndex == boidIndex)
                    continue;

                /*if (alignCount >= sim->maxVisible)
                    goto done_neighbors;*/

                const Boid* other = &current[otherIndex];

                vec2 otherPos = { other->x, other->y };
                real distanceSq = vec_dist_sq(boidPos, otherPos);

                if (distanceSq <= R(0.0) || distanceSq >= sim->visionRadiusSq)
                    continue;

                if (distanceSq < sim->protectedRangeSq)
                {
                    vec2 away = vec_sub(boidPos, otherPos);
                    away = vec_mul(away, R(1.0) / distanceSq);

                    avoid = vec_add(avoid, away);
                    avoidCount++;
                }

                align = vec_add(align, other->speed);
                alignCount++;

                cohere = vec_add(cohere, otherPos);
                cohesionCount++;
            }
        }
    }

done_neighbors:

    if (avoidCount > 0)
    {
        avoid = vec_mul(avoid, R(1.0) / R(avoidCount));
        avoid = vec_norm(avoid);
    }

    if (alignCount > 0)
    {
        align = vec_mul(align, R(1.0) / R(alignCount));
        align = vec_sub(align, boid->speed);
        align = vec_norm(align);
    }

    if (cohesionCount > 0)
    {
        cohere = vec_mul(cohere, R(1.0) / R(cohesionCount));
        cohere = vec_sub(cohere, boidPos);
        cohere = vec_norm(cohere);
    }

    vec2 flockForce = { 0, 0 };

    flockForce = vec_add(flockForce, vec_mul(avoid, sim->avoidFactor));
    flockForce = vec_add(flockForce, vec_mul(align, sim->matchingFactor));
    flockForce = vec_add(flockForce, vec_mul(cohere, sim->centeringFactor));

    vec2 wallForce = AvoidBorders(boid, BORDER_MARGIN);
	vec2 obstacleForce = AvoidObstacle(boid, obstacles, sim->obstacleAvoidDistance);
    vec2 poiForce = { 0, 0 };

    PointOfInterest* closestPOI = NULL;
    real closestDistSq = REAL_MAX;

    for (int i = 0; i < poiCount; i++)
    {
        if (!pointsOfInterest[i].active)
            continue;

        vec2 offset = poi_get_direction_vector(&pointsOfInterest[i], boid);
        real distSq = vec_mag_sq(offset);

        if (distSq < closestDistSq)
        {
            closestDistSq = distSq;
            closestPOI = &pointsOfInterest[i];
        }
    }

    if (closestPOI != NULL)
    {
        poiForce = poi_get_force(closestPOI, boid, sim);
    }

    flockForce = vec_add(flockForce, vec_mul(poiForce, sim->poiFactor));
    flockForce = vec_add(flockForce, vec_mul(wallForce, sim->borderingFactor));
    flockForce = vec_add(flockForce, vec_mul(obstacleForce, sim->obstacleAvoidFactor));

    vec2 acceleration = vec_mul(flockForce, R(1.0) / sim->turnSpeed);

    boid->speed = vec_add(
        boid->speed,
        vec_mul(acceleration, deltaTime)
    );

    boid->speed = vec_clamp_mag(
        boid->speed,
        R(sim->minSpeed),
        R(sim->topSpeed)
    );

    /*if (vec_mag(boid->speed) > R(0.01))
    {
        boid->angle =
            REAL_ATAN2(boid->speed.y, boid->speed.x) *
            R(180.0) / REAL_PI;
    }*/
}

void Flock(Boid* boid, Boid* boids, int numBoids, SimulationParameters* sim, PointOfInterest* pointsOfInterest, int poiCount, real deltaTime) {
    vec2 avoid = { 0, 0 };
    vec2 align = { 0, 0 };
    vec2 cohere = { 0, 0 };

    int avoidCount = 0;
    int alignCount = 0;
    int cohesionCount = 0;

    vec2 boidPos = { boid->x, boid->y };

    for (int i = 0; i < numBoids; i++)
    {
        Boid* other = &boids[i];

        if (other == boid)
        {
            continue;
        }

        if (alignCount >= sim->maxVisible)
        {
            break;
        }

        vec2 otherPos = { other->x, other->y };

		real distanceSq = vec_dist_sq(boidPos, otherPos);
        if (distanceSq <= 0.0 || distanceSq >= sim->visionRadiusSq)
        {
            continue;
		}

        // Avoidance
        if (distanceSq < sim->protectedRangeSq)
        {
            vec2 away = vec_sub(boidPos, otherPos);

            // Stronger avoidance when very close
            away = vec_mul(away, ((real)1.0) / distanceSq);

            avoid = vec_add(avoid, away);
            avoidCount++;
        }

        // Alignment
        align = vec_add(align, other->speed);
        alignCount++;

        // Cohesion
        cohere = vec_add(cohere, otherPos);
        cohesionCount++;
    }

    if (avoidCount > 0)
    {
        avoid = vec_mul(avoid, R(1.0) / R(avoidCount));
        avoid = vec_norm(avoid);
    }

    if (alignCount > 0)
    {
        align = vec_mul(align, R(1.0) / R(alignCount));
        align = vec_sub(align, boid->speed);
        align = vec_norm(align);
    }

    if (cohesionCount > 0)
    {
        cohere = vec_mul(cohere, R(1.0) / R(cohesionCount));
        cohere = vec_sub(cohere, boidPos);
        cohere = vec_norm(cohere);
    }

    vec2 flockForce = { 0, 0 };

    flockForce = vec_add(flockForce, vec_mul(avoid, sim->avoidFactor));
    flockForce = vec_add(flockForce, vec_mul(align, sim->matchingFactor));
    flockForce = vec_add(flockForce, vec_mul(cohere, sim->centeringFactor));

    vec2 wallForce = AvoidBorders(boid, BORDER_MARGIN);
	vec2 poiForce = { 0, 0 };

    PointOfInterest* closestPOI = NULL;
    real closestDistSq = REAL_MAX;

    for (int i = 0; i < poiCount; i++)
    {
        if (!pointsOfInterest[i].active)
        {
            continue;
        }

        vec2 offset = poi_get_direction_vector(&pointsOfInterest[i], boid);
        real distSq = vec_mag_sq(offset);

        if (distSq < closestDistSq)
        {
            closestDistSq = distSq;
            closestPOI = &pointsOfInterest[i];
        }
    }
    if (closestPOI != NULL)
    {
        poiForce = poi_get_force(closestPOI, boid, sim);
        //consume_poi(closestPOI, boid, 1);
    }

    flockForce = vec_add(flockForce, vec_mul(poiForce, sim->poiFactor));

    flockForce = vec_add(flockForce, vec_mul(wallForce, sim->borderingFactor));


    vec2 acceleration = vec_mul(flockForce, R(1.0) / sim->turnSpeed);

    boid->speed = vec_add(boid->speed, vec_mul(acceleration, deltaTime));

    boid->speed = vec_clamp_mag(
        boid->speed,
        R(sim->minSpeed),
        R(sim->topSpeed)
    );

    /*if (vec_mag(boid->speed) > 0.01)
    {
        boid->angle = REAL_ATAN2(boid->speed.y, boid->speed.x) * R(180.0) / REAL_PI;
    }*/
}


vec2 AvoidBorders(Boid* boid, real borderMargin)
{
    vec2 force = { 0, 0 };

    if (boid->x < BORDER_MARGIN)
    {
        real t = (BORDER_MARGIN - boid->x) / borderMargin;
        force.x += t * t;
    }
    else if (boid->x > WORLD_WIDTH - BORDER_MARGIN)
    {
        real t = (boid->x - (WORLD_WIDTH - BORDER_MARGIN)) / borderMargin;
        force.x -= t * t;
    }

    if (boid->y < BORDER_MARGIN)
    {
        real t = (BORDER_MARGIN - boid->y) / borderMargin;
        force.y += t * t;
    }
    else if (boid->y > WORLD_HEIGHT - BORDER_MARGIN)
    {
        real t = (boid->y - (WORLD_HEIGHT - BORDER_MARGIN)) / borderMargin;
        force.y -= t * t;
    }

    return force;
}

vec2 AvoidObstacle(Boid* boid, Obstacles* obstacles, real avoidDistance)
{
    vec2 force = { 0, 0 };
    vec2 p = { boid->x, boid->y };

    real avoidDistanceSq = avoidDistance * avoidDistance;

    for (Obstacles* obstacle = obstacles; obstacle != NULL; obstacle = obstacle->next)
    {
        vec2 a = { obstacle->x1, obstacle->y1 };
        vec2 b = { obstacle->x2, obstacle->y2 };

        vec2 ab = vec_sub(b, a);
        vec2 ap = vec_sub(p, a);

        real abLenSq = vec_mag_sq(ab);

        if (abLenSq <= R(0.0001))
            continue;

        real t = vec_dot(ap, ab) / abLenSq;
        t = CLAMP(t, R(0.0), R(1.0));

        vec2 closest = vec_add(a, vec_mul(ab, t));
        vec2 away = vec_sub(p, closest);

        real distSq = vec_mag_sq(away);

        if (distSq <= R(0.0001) || distSq > avoidDistanceSq)
            continue;

        real dist = REAL_SQRT(distSq);

        vec2 dir = vec_mul(away, R(1.0) / dist);

        real strength = R(1.0) - (dist / avoidDistance);
        strength = strength * strength;

        force = vec_add(force, vec_mul(dir, strength));
    }

    return force;
}




FlockJob initFlockJob(int startIndex, int endIndex, const Boid* current, Boid* next, UniformGrid* grid, Obstacles* obstacles, SimulationParameters* sim, PointOfInterest* pois, int poiCount, real deltaTime)
{
    FlockJob flockJob = {
        .startIndex = startIndex,
        .endIndex = endIndex,
        .current = current,
        .next = next,
        .grid = grid,
		.obstacles = obstacles,
        .sim = sim,
        .pois = pois,
        .poiCount = poiCount,
        .deltaTime = deltaTime
    };

    return flockJob;
}

int WorkerMain(void* data)
{
    FlockJob* job = (FlockJob*)data;

    if (job == NULL)
    {
        return 1;
    }

    for (int i = job->startIndex; i < job->endIndex; i++)
    {
        Boid updated = job->current[i];

        FlockGrid(
            i,
            job->current,
            &updated,
            job->grid,
			job->obstacles,
            job->sim,
            job->pois,
            job->poiCount,
            job->deltaTime
        );

        updated.x += updated.speed.x * job->deltaTime;
        updated.y += updated.speed.y * job->deltaTime;

        if (updated.x < 0) updated.x += WORLD_WIDTH;
        if (updated.x > WORLD_WIDTH) updated.x -= WORLD_WIDTH;

        if (updated.y < 0) updated.y += WORLD_HEIGHT;
        if (updated.y > WORLD_HEIGHT) updated.y -= WORLD_HEIGHT;
        job->next[i] = updated;
    }

    return 0;
}

void FlockJob_Run(void* data, int start, int end, int threadIndex)
{
    FlockJob* job = (FlockJob*)data;

    FlockJob chunkJob = *job;
    chunkJob.startIndex = start;
    chunkJob.endIndex = end;

    WorkerMain(&chunkJob);
}


