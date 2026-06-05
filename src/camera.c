#include "camera.h"

vec2 WorldToScreen(Camera* cam, vec2 world) {
    return (vec2) {
        (world.x - cam->x) * cam->zoom,
        (world.y - cam->y) * cam->zoom
    };
}

vec2 ScreenToWorld(Camera* cam, vec2 screen) {
    return (vec2) {
        screen.x / cam->zoom + cam->x,
            screen.y / cam->zoom + cam->y
    };
}

void Camera_ZoomAt(Camera* cam, vec2 screenPoint, real zoomFactor)
{
    vec2 before = ScreenToWorld(cam, screenPoint);

    cam->zoom *= zoomFactor;

    if (cam->zoom < cam->minZoom) cam->zoom = cam->minZoom;
    if (cam->zoom > cam->maxZoom) cam->zoom = cam->maxZoom;

    vec2 after = ScreenToWorld(cam, screenPoint);

    cam->x += before.x - after.x;
    cam->y += before.y - after.y;
}