
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
