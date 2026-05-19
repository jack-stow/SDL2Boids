/*
 * Copyright (C) 2015-2018,2022 Parallel Realities. All rights reserved.
 */

#include <SDL2/SDL_image.h>

#include "common.h"

#include "draw.h"

extern App app;

void prepareScene(void)
{
	SDL_SetRenderDrawColor(app.renderer, 96, 128, 255, 255);
	SDL_RenderClear(app.renderer);
}

void presentScene(void)
{
	SDL_RenderPresent(app.renderer);
}

SDL_Texture* loadTexture(char* filename)
{
	SDL_Texture* texture;

	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading %s", filename);

	texture = IMG_LoadTexture(app.renderer, filename);

	return texture;
}

void blit(SDL_Texture* texture, int x, int y, double angle, double scale)
{
	SDL_Rect dest;
	int w, h;

	SDL_QueryTexture(texture, NULL, NULL, &w, &h);

	dest.x = x;
	dest.y = y;
	dest.w = (int)(w * scale);
	dest.h = (int)(h * scale);

	SDL_RenderCopyEx(app.renderer, texture, NULL, &dest, angle, NULL, SDL_FLIP_NONE);
}

/// <summary>
/// currently does not draw a circle. It draws a square. TODO: fix.
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <param name="radius"></param>
void draw_circle(double x, double y, double radius)
{
	SDL_Rect rect;

	rect.w = (int)(radius * 2);
	rect.h = (int)(radius * 2);
	rect.x = (int)(x - rect.w / 2);
	rect.y = (int)(y - rect.h / 2);

	SDL_SetRenderDrawColor(app.renderer, 0, 100, 255, 255);
	SDL_RenderFillRect(app.renderer, &rect);
}