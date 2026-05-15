/*
 * Copyright (C) 2015-2018,2022 Parallel Realities. All rights reserved.
 */

#include "common.h"

#include "draw.h"
#include "init.h"
#include "input.h"
#include "main.h"
#include "player.h"

App    app;
Entity player;

int main(int argc, char* argv[])
{
	memset(&app, 0, sizeof(App));
	memset(&player, 0, sizeof(Entity));

	initSDL();

	atexit(cleanup);

	int topSpeed = 8;
	double posX = 100.0;
	double posY = 100.0;
	vec2 speedNormalized = { 0, 0 };


	initPlayer(&player, posX, posY, topSpeed, "gfx/boid.png");

	while (1)
	{
		prepareScene();

		doInput();

		updatePlayer(&player, (vec2) {
			(double)(app.right - app.left),
				(double)(app.down - app.up)
		});

		drawPlayer(&player);

		presentScene();

		SDL_Delay(16);
		
	}

	return 0;
}
