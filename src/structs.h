/*
 * Copyright (C) 2015-2018,2022 Parallel Realities. All rights reserved.
 */
#ifndef STRUCTS_H
#define STRUCTS_H
typedef struct {
    SDL_Renderer *renderer;
    SDL_Window *window;
    int up;
    int down;
    int left;
    int right;
} App;
#endif
