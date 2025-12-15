#pragma once

#include <string>

#include <SDL3/SDL.h>
#include <glad/gl.h>

#include "types/camera.h"
#include "renderer.h"

#include <memory>
#include <unordered_map>

class Application
{
public:
    Application(std::string title, uint32_t width, uint32_t height);
    ~Application();

    void run();

private:
    void handleInput();
    void update();
    void render();

    SDL_Window *window = nullptr;

    Camera camera;
    std::unique_ptr<Renderer> renderer;

    Scene scene;
    std::unordered_map<std::string, Model> models;

    std::string title = "";
    uint32_t    width = 1280;
    uint32_t    height = 720;

    bool running = false;
    bool minimized = false;

    float delta = 0.0f;
    float time = 0.0f;
};