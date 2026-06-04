#include "common.h"

#include "input.h"

extern App app;

static void doKeyUp(SDL_KeyboardEvent* event)
{
	if (event->repeat == 0)
	{
		if (event->keysym.scancode == SDL_SCANCODE_UP)
		{
			app.up = 0;
		}

		if (event->keysym.scancode == SDL_SCANCODE_DOWN)
		{
			app.down = 0;
		}

		if (event->keysym.scancode == SDL_SCANCODE_LEFT)
		{
			app.left = 0;
		}

		if (event->keysym.scancode == SDL_SCANCODE_RIGHT)
		{
			app.right = 0;
		}
	}
}

static void doKeyDown(SDL_KeyboardEvent* event)
{
	if (event->repeat == 0)
	{
		if (event->keysym.scancode == SDL_SCANCODE_UP)
		{
			app.up = 1;
		}

		if (event->keysym.scancode == SDL_SCANCODE_DOWN)
		{
			app.down = 1;
		}

		if (event->keysym.scancode == SDL_SCANCODE_LEFT)
		{
			app.left = 1;
		}

		if (event->keysym.scancode == SDL_SCANCODE_RIGHT)
		{
			app.right = 1;
		}
	}
}

static void doMouseButtonDown(SDL_MouseButtonEvent* event)
{
	if (event->button == SDL_BUTTON_LEFT) {
		app.mouseDown = 1;
		app.dragStartX = event->x;
		app.dragStartY = event->y;
		app.mouseX = event->x;
		app.mouseY = event->y;
	}
	if (event->button == SDL_BUTTON_RIGHT) {
		app.rmouseDown = 1;
		app.rdragStartX = event->x;
		app.rdragStartY = event->y;
	}
}

static void doMouseButtonUp(SDL_MouseButtonEvent* event)
{
	if (event->button == SDL_BUTTON_LEFT) {
		app.mouseDown = 0;
		app.dragEndX = event->x;
		app.dragEndY = event->y;
	}
	if (event->button == SDL_BUTTON_RIGHT) {
		app.rmouseDown = 0;
		app.rdragEndX = event->x;
		app.rdragEndY = event->y;
	}
}	

void doInput(void)
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_QUIT:
				exit(0);
				break;
			
			case SDL_KEYDOWN:
				doKeyDown(&event.key);
				break;

			case SDL_KEYUP:
				doKeyUp(&event.key);
				break;

			case SDL_MOUSEBUTTONDOWN:
				doMouseButtonDown(&event.button);
				break;

			case SDL_MOUSEBUTTONUP:
				doMouseButtonUp(&event.button);
				break;

			case SDL_MOUSEMOTION:
				app.mouseX = event.motion.x;
				app.mouseY = event.motion.y;

				app.prevRMouseX = app.rmouseX;
				app.prevRMouseY = app.rmouseY;

				app.rmouseX = event.motion.x;
				app.rmouseY = event.motion.y;

				break;

			default:
				break;
		}
	}
}
