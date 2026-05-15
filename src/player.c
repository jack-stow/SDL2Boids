
#include "player.h"

void initPlayer(Entity* player, double posX, double posY, double topSpeed, double acceleration, char* texture)
{
    player->x = posX;
    player->y = posY;
    player->speed.x = 0;
    player->speed.y = 0;
    player->desiredSpeed.x = 0;
    player->desiredSpeed.y = 0;
    player->angle = 0;
    player->desiredAngle = 0;
	player->topSpeed = topSpeed;
	player->acceleration = acceleration;
	player->texture = loadTexture(texture);
    if (player->texture == NULL)
    {
        SDL_Log("Failed to load player texture: %s", texture);
    }
}

void updatePlayer(Entity* player, vec2 inputDirection)
{
    vec2 desiredSpeed = { 0, 0 };

    desiredSpeed.x = (double)(inputDirection.x);
    desiredSpeed.y = (double)(inputDirection.y);

    desiredSpeed = vec_mul(vec_norm(desiredSpeed), player->topSpeed);

    player->speed = vec_lerp(player->speed, desiredSpeed, player->acceleration);

    if (vec_mag(player->speed) > 0.01)
    {
        player->desiredAngle = atan2(player->speed.y, player->speed.x) * 180.0 / M_PI;
    }

    player->angle = normalize_angle(
        angle_lerp(player->angle, player->desiredAngle, 0.25)
    );
	//player->angle = player->desiredAngle;

    player->x = CLAMP(player->x + player->speed.x, 0, SCREEN_WIDTH - 48.0);
    player->y = CLAMP(player->y + player->speed.y, 0, SCREEN_HEIGHT - 48.0);
}

void drawPlayer(Entity* player)
{
    blit(player->texture, player->x, player->y, player->angle);
}
