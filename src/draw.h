/*
 * Copyright (C) 2015-2018,2022 Parallel Realities. All rights reserved.
 */

void prepareScene(void);
void presentScene(void);
void blit(SDL_Texture* texture, int x, int y, double angle);
SDL_Texture* loadTexture(char* filename);