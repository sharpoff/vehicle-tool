#pragma once

#include "math_defines.h"

#include <SDL3/SDL.h>

struct Camera
{
    mat4 view = mat4(1.0);
    mat4 projection = mat4(1.0);
    vec3 position = vec3(0.0);

    vec3 up = vec3(0, 1, 0);
    vec3 right = vec3(1, 0, 0);

    float yaw = -90.0f;
    float pitch = 0.0f;

    float moveSpeed = 4.5;
    float mouseSpeed = 0.5;

    struct {
        bool left;
        bool right;
        bool up;
        bool down;
    } keys;

    void processEvent(SDL_Event *event, float delta);
    void update(float delta);
    void setProjection(float fov, float aspect, float near, float far);
};
