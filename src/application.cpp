#include "application.h"

#include <chrono>
#include <stdio.h>

#include "renderer.h"
#include "gl_utility.h"
#include "obj_loader.h"
#include "render_flags.h"
#include "types/entity.h"
#include "types/light.h"

#include "ImGuizmo.h"
#include "imgui_impl_sdl3.h"

#include <SDL3/SDL_events.h>

Application::Application(std::string title, uint32_t width, uint32_t height)
    : title(title), width(width), height(height)
{
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        printf("Failed to initialize SDL. %s\n", SDL_GetError());
        exit(-1);
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    window = SDL_CreateWindow(title.c_str(), width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!window) {
        printf("Failed to create SDL window. %s\n", SDL_GetError());
        exit(-1);
    }

    renderer = std::make_unique<Renderer>(window, camera);

    camera.setProjection(glm::radians(65.0f), float(width) / height, 0.1f, 100.0f);
    camera.position = vec3(0.0f, 2.0f, 3.0f);

    
    // load models
    if (obj::loadObj(models["cube"], "models/cube.obj")) {
        models["cube"].setCustomMaterial({
            .baseColor = vec3(1.0f)
        });
    }

    if (obj::loadObj(models["car"], "models/auweschveb_vehicle_volgohod.obj")) {
        models["car"].setCustomMaterial({
            .baseColorTexture = gl::createTextureFromFile("textures/auweschveb_vehicle_volgohod_black.png")
        });
    }

    // add entities
    if (EntityId entityId = scene.addLight(); entityId != EntityId::Invalid) {
        Light *light = static_cast<Light*>(scene.getEntityById(entityId));
        light->setWorldMatrix(glm::translate(vec3(-3.0f, 5.0f, 3.0f)) * glm::scale(vec3(0.2)));
        light->color = vec3(1.0f, 0.0f, 0.0f);
        light->type = LightType::Directional;
    }

    if (EntityId entityId = scene.addLight(); entityId != EntityId::Invalid) {
        Light *light = static_cast<Light*>(scene.getEntityById(entityId));
        light->setWorldMatrix(glm::translate(vec3(0.0f, 5.0f, 3.0f)) * glm::scale(vec3(0.2)));
        light->color = vec3(1.0f, 0.0f, 0.0f);
        light->type = LightType::Directional;
    }

    if (EntityId entityId = scene.addEntity(EntityType::Model, "car"); entityId != EntityId::Invalid) {
        Entity *entity = static_cast<Entity*>(scene.getEntityById(entityId));
        entity->setModel(models["car"]);
    }
}

Application::~Application()
{
    renderer.reset();

    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Application::run()
{
    auto previous = std::chrono::high_resolution_clock::now();

    running = true;
    while (running) {
        auto current = std::chrono::high_resolution_clock::now();
        delta = std::chrono::duration<float>(current - previous).count();
        time += delta;
        previous = current;

        handleInput();
        update();
        render();
    }
}

void Application::handleInput()
{
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
        ImGui_ImplSDL3_ProcessEvent(&event);

        switch (event.type) {
            case SDL_EVENT_QUIT: {
                running = false;
                break;
            }
            case SDL_EVENT_KEY_UP:
            case SDL_EVENT_KEY_DOWN: {
                if (event.key.key == SDLK_ESCAPE) {
                    running = false;
                }
                if (event.key.key == SDLK_P) {
                    renderer->reloadShaders();
                }
                if (event.key.key == SDLK_T) {
                    renderer->setGizmoOperation(ImGuizmo::TRANSLATE);
                }
                if (event.key.key == SDLK_R) {
                    renderer->setGizmoOperation(ImGuizmo::ROTATE);
                }
                if (event.key.key == SDLK_E) {
                    renderer->setGizmoOperation(ImGuizmo::SCALE);
                }
                break;
            }
            case SDL_EVENT_WINDOW_RESIZED: {
                uint32_t width = 0, height = 0;
                SDL_GetWindowSize(window, (int*)&width, (int*)&height);

                renderer->resizeViewport(width, height);
                break;
            }
        }

        camera.processEvent(&event, delta);
    }

    if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED) {
        minimized = true;
    } else {
        minimized = false;
    }
}

void Application::update()
{
    camera.update(delta);
}

void Application::render()
{
    if (minimized)
        return;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (Entity *entity : scene.getEntitiesByType(EntityType::Light)) {
        Light *light = static_cast<Light*>(entity);
        renderer->renderModel(models["cube"], RENDER_COLOR, light->getWorldMatrix());
    }

    renderer->renderScene(scene);
    renderer->renderUI(scene);

    SDL_GL_SwapWindow(window);
}