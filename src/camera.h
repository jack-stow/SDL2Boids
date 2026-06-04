#pragma once
#include "types.h"
#include "vector2.h"
typedef struct Camera {
    real x, y;        // world-space top-left, or center
    real zoom;        // pixels per world unit
    int screenW, screenH;
} Camera;



vec2 WorldToScreen(Camera* cam, vec2 world);

vec2 ScreenToWorld(Camera* cam, vec2 screen);