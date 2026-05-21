
#include "player.h"

void initPlayer(Entity* player, real posX, real posY, real topSpeed, real acceleration, char* texture)
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

    desiredSpeed.x = R(inputDirection.x);
    desiredSpeed.y = R(inputDirection.y);

    desiredSpeed = vec_mul(vec_norm(desiredSpeed), player->topSpeed);

    player->speed = vec_lerp(player->speed, desiredSpeed, player->acceleration);

    if (vec_mag(player->speed) > 0.01)
    {
        player->desiredAngle = REAL_ATAN2(player->speed.y, player->speed.x) * R(180.0) / REAL_PI;
    }

    player->angle = normalize_angle(
        angle_lerp(player->angle, player->desiredAngle, 0.25)
    );
	//player->angle = player->desiredAngle;

    player->x = R(CLAMP(player->x + player->speed.x, 0, SCREEN_WIDTH - 48.0));
    player->y = R(CLAMP(player->y + player->speed.y, 0, SCREEN_HEIGHT - 48.0));
}

void drawPlayer(Entity* player)
{
    blit(player->texture, player->x, player->y, player->angle, 0.5);
}
