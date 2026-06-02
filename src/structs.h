
#ifndef STRUCTS_H
#define STRUCTS_H
#include <stdbool.h>
typedef struct {
    SDL_Renderer* renderer;
    SDL_Window* window;

    int up, down, left, right;

    int mouseDown;
    int mouseX, mouseY;

    int dragStartX, dragStartY;
    int dragEndX, dragEndY;

    int rmouseDown;
    int rmouseX, rmouseY;

    int rdragStartX, rdragStartY;
    int rdragEndX, rdragEndY;
} App;
#endif
