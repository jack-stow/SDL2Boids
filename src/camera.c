#include "camera.h"

vec2 WorldToScreen(Camera* cam, vec2 world) {
    return (vec2) {
        (world.x - cam->x)* cam->zoom,
        (world.y - cam->y)* cam->zoom
    };
}

vec2 ScreenToWorld(Camera* cam, vec2 screen) {
    return (vec2) {
        screen.x / cam->zoom + cam->x,
            screen.y / cam->zoom + cam->y
    };
}