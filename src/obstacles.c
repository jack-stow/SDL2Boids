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

void Obstacles_Draw(Obstacles* obstacles)
{
	if (obstacles == NULL) return;
	Obstacles* next = obstacles;
	DrawColor color = {255, 255, 255, 255};
	while (next != NULL) {
		draw_line(next->x1, next->y1, next->x2, next->y2, color);
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

	Obstacles* next = *obstacles;
	Obstacles* prev = NULL;

	while (next != NULL) {
		Obstacles* after = next->next;

		if (Obstacle_IntersectsLine(next, x1, y1, x2, y2)) {
			if (prev == NULL) {
				*obstacles = after;
			}
			else {
				prev->next = after;
			}

			free(next);
			return;
		}

		prev = next;
		next = after;
	}
}