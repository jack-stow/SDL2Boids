#include "obstacles.h"
#include "draw.h"

void Obstacles_Add(Obstacles** obstacles, real x1, real y1, real x2, real y2)
{
	if (obstacles == NULL) return;

	Obstacles* newNode = malloc(sizeof(Obstacles));
	if (newNode == NULL) return;

	newNode->x1 = x1;
	newNode->y1 = y1;
	newNode->x2 = x2;
	newNode->y2 = y2;
	newNode->next = NULL;

	if (*obstacles == NULL) {
		*obstacles = newNode;
		return;
	}

	Obstacles* next = *obstacles;

	while (next->next != NULL) {
		next = next->next;
	}

	next->next = newNode;
}

void Obstacles_Free(Obstacles* obstacles)
{
	if (obstacles == NULL) return;
	Obstacles* next = obstacles;
	while (next != NULL) {
		Obstacles* toFree = next;
		next = next->next;
		free(toFree);
	}
}

void Obstacles_RemoveOne(Obstacles* obstacles, Obstacles* toRemove)
{
	if (obstacles == NULL || toRemove == NULL) return;
	Obstacles* next = obstacles;
	Obstacles* prev = NULL;
	while (next != NULL) {
		if (next == toRemove) {
			if (prev == NULL) {
				obstacles = next->next;
			}
			else {
				prev->next = next->next;
			}
			free(next);
			return;
		}
		prev = next;
		next = next->next;
	}
}

void Obstacles_Draw(Camera* camera, Obstacles* obstacles)
{
	if (obstacles == NULL) return;
	Obstacles* next = obstacles;
	DrawColor color = {255, 255, 255, 255};
	while (next != NULL) {
		vec2 screenPos1 = WorldToScreen(camera, (vec2) { next->x1, next->y1 });
		vec2 screenPos2 = WorldToScreen(camera, (vec2) { next->x2, next->y2 });
		draw_line(screenPos1.x, screenPos1.y, screenPos2.x, screenPos2.y, color);
		next = next->next;
	}
}

int Obstacles_Count(const Obstacles* obstacles)
{
	if (obstacles == NULL) return 0;
	Obstacles* next = obstacles;
	int count = 0;
	while (next != NULL) {
		count++;
		next = next->next;
	}
    return count;
}

bool Obstacle_IntersectsLine(const Obstacles* obstacles, real x1, real y1, real x2, real y2)
{
	if (obstacles == NULL) return false;
	return line_segments_intersect(
		(vec2){x1, y1},
		(vec2){x2, y2},
		(vec2){obstacles->x1, obstacles->y1},
		(vec2){obstacles->x2, obstacles->y2}
	);
}

void Obstacles_EraseIntersecting(Obstacles** obstacles, real x1, real y1, real x2, real y2)
{
	if (obstacles == NULL || *obstacles == NULL) return;

	Obstacles* current = *obstacles;
	Obstacles* previous = NULL;

	while (current != NULL)
	{
		// Use vec2 for line_segments_intersect as required by its signature
		vec2 a1 = { x1, y1 };
		vec2 a2 = { x2, y2 };
		vec2 b1 = { current->x1, current->y1 };
		vec2 b2 = { current->x2, current->y2 };

		if (line_segments_intersect(a1, a2, b1, b2))
		{
			Obstacles* toDelete = current;

			if (previous == NULL)
			{
				*obstacles = current->next;
				current = *obstacles;
			}
			else
			{
				previous->next = current->next;
				current = current->next;
			}

			free(toDelete);
		}
		else
		{
			previous = current;
			current = current->next;
		}
	}
}