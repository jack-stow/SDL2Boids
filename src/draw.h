#pragma once
#include "common.h"

typedef struct
{
	Uint8 r, g, b, a;
} DrawColor;

void prepareScene(void);
void presentScene(void);
void blit(SDL_Texture* texture, real x, real y, real angle, real scale);
SDL_Texture* loadTexture(char* filename);
void draw_circle(real x, real y, real radius, DrawColor color, bool filled);
void draw_line(real x1, real y1, real x2, real y2, DrawColor color);