#pragma once
#include "types.h"
#include "vector2.h"


#define CAMERA_SPEED 2000.0

typedef struct Camera {
    real x, y;        // world-space top-left, or center
    real zoom;        // pixels per world unit
    int screenW, screenH;

    float minZoom;
    float maxZoom;
} Camera;



vec2 WorldToScreen(Camera* cam, vec2 world);

vec2 ScreenToWorld(Camera* cam, vec2 screen);


void Camera_ZoomAt(Camera* cam, vec2 screenPoint, real zoomFactor);