#pragma once

typedef struct
{
	Uint8 r, g, b, a;
} Color;

void prepareScene(void);
void presentScene(void);
void blit(SDL_Texture* texture, real x, real y, real angle, real scale);
SDL_Texture* loadTexture(char* filename);
void draw_circle(real x, real y, real radius, Color color, bool filled);