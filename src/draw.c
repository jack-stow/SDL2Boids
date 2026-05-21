
#include <SDL2/SDL_image.h>

#include "common.h"

#include "draw.h"

extern App app;

void prepareScene(void)
{
    SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 255);
	SDL_SetRenderDrawBlendMode(app.renderer, SDL_BLENDMODE_BLEND);
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



void blit(SDL_Texture* texture, real x, real y, real angle, real scale)
{
	SDL_Rect dest;
	int w, h;

	SDL_QueryTexture(texture, NULL, NULL, &w, &h);

    dest.w = (int)(w * scale);
    dest.h = (int)(h * scale);

    dest.x = (int)(x - dest.w / 2);
    dest.y = (int)(y - dest.h / 2);


	SDL_RenderCopyEx(app.renderer, texture, NULL, &dest, angle, NULL, SDL_FLIP_NONE);
}


//void draw_rect(real x, real y, real radius)
//{
//	SDL_Rect rect;
//
//	rect.w = (int)(radius * 2);
//	rect.h = (int)(radius * 2);
//	rect.x = (int)(x - rect.w / 2);
//	rect.y = (int)(y - rect.h / 2);
//
//	SDL_SetRenderDrawColor(app.renderer, 0, 100, 255, 255);
//	SDL_RenderFillRect(app.renderer, &rect);
//}

void draw_circle(real x, real y, real radius, Color color, bool filled)
{
    SDL_SetRenderDrawColor(app.renderer, color.r, color.g, color.b, color.a);
    SDL_SetRenderDrawBlendMode(app.renderer, SDL_BLENDMODE_BLEND);
    int cx = (int)x;
    int cy = (int)y;
    int r = (int)radius;

    for (int dy = -r; dy <= r; dy++)
    {
        for (int dx = -r; dx <= r; dx++)
        {
            int distSq = dx * dx + dy * dy;
            int radiusSq = r * r;

            if (filled)
            {
                if (distSq <= radiusSq)
                {
                    SDL_RenderDrawPoint(app.renderer, cx + dx, cy + dy);
                }
            }
            else
            {
                // outline thickness tolerance
                if (distSq >= radiusSq - r &&
                    distSq <= radiusSq + r)
                {
                    SDL_RenderDrawPoint(app.renderer, cx + dx, cy + dy);
                }
            }
        }
    }
}
