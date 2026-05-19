#pragma once

typedef struct
{
	Uint8 r, g, b, a;
} Color;

void prepareScene(void);
void presentScene(void);
void blit(SDL_Texture* texture, int x, int y, double angle, double scale);
SDL_Texture* loadTexture(char* filename);
void draw_circle(double x, double y, double radius, Color color, bool filled);