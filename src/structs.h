
#ifndef STRUCTS_H
#define STRUCTS_H
#include <stdbool.h>
typedef struct {
    SDL_Renderer* renderer;
    SDL_Window* window;

    int up, down, left, right;

    int mouseDown;
    double mouseX, mouseY;

    double dragStartX, dragStartY;
    double dragEndX, dragEndY;

    int rmouseDown;
    double rmouseX, rmouseY;

    double prevRMouseX, prevRMouseY;

    double rdragStartX, rdragStartY;
    double rdragEndX, rdragEndY;

    double mouseWheelY;
} App;
#endif
