#include "types/camera.h"

#include "imgui.h"

void Camera::processEvent(SDL_Event *event, float delta)
{
    if (event->type == SDL_EVENT_KEY_DOWN || event->key.repeat) {
        if (event->key.key == SDLK_A) keys.left = true;
        if (event->key.key == SDLK_D) keys.right = true;
        if (event->key.key == SDLK_W) keys.up = true;
        if (event->key.key == SDLK_S) keys.down = true;
    }

    if (event->type == SDL_EVENT_KEY_UP) {
        if (event->key.key == SDLK_A) keys.left = false;
        if (event->key.key == SDLK_D) keys.right = false;
        if (event->key.key == SDLK_W) keys.up = false;
        if (event->key.key == SDLK_S) keys.down = false;
    }

    float x, y;
    if (!ImGui::GetIO().WantCaptureMouse && SDL_GetMouseState(&x, &y) & SDL_BUTTON_LMASK && event->type == SDL_EVENT_MOUSE_MOTION) {
        yaw += event->motion.xrel * mouseSpeed;
        pitch -= event->motion.yrel * mouseSpeed;
    }
}

void Camera::update(float delta)
{
    // spherical coords to cartesian
    vec3 direction = {
        cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
        sin(glm::radians(pitch)),
        sin(glm::radians(yaw)) * cos(glm::radians(pitch)),
    };
    direction = glm::normalize(direction);

    right = glm::cross(direction, up);

    if (keys.up) {
        position += direction * moveSpeed * delta;
    }
    if (keys.down) {
        position -= direction * moveSpeed * delta;
    }
    if (keys.left) {
        position -= right * moveSpeed * delta;
    }
    if (keys.right) {
        position += right * moveSpeed * delta;
    }

    view = glm::lookAt(position, position + direction, up);
}

void Camera::setProjection(float fov, float aspect, float near, float far)
{
    projection = glm::perspective(fov, aspect, near, far);
}