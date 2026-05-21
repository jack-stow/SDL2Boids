#include "common.h"

typedef struct
{
	real          x;
	real          y;

	vec2 speed;
	vec2 desiredSpeed;

	real angle;
	real desiredAngle;

	real topSpeed;
	real acceleration;


	SDL_Texture* texture;
} Entity;



void initPlayer(Entity* player, real posX, real posY, real topSpeed, real acceleration, char* texture);
void updatePlayer(Entity* player, vec2 inputDirection);
void drawPlayer(Entity* player);