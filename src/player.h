#include "common.h"

typedef struct
{
	double          x;
	double          y;

	vec2 speed;
	vec2 desiredSpeed;

	double angle;
	double desiredAngle;

	double topSpeed;


	SDL_Texture* texture;
} Entity;



void initPlayer(Entity* player, double posX, double posY, double topSpeed, char* texture);
void updatePlayer(Entity* player, vec2 inputDirection);
void drawPlayer(Entity* player);