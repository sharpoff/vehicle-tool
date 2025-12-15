#pragma once

#include "render_flags.h"
#include <string>

#include "types/camera.h"
#include "types/model.h"
#include "types/scene.h"

#include <SDL3/SDL_video.h>

#include "ImGuizmo.h"

class Renderer
{
public:
    Renderer(SDL_Window *window, Camera &camera);
    ~Renderer();

    void renderUI(Scene &scene);
    void renderScene(Scene &scene);
    void renderModel(Model &model, RenderFlags renderFlags, mat4 worldMatrix = mat4(1.0f));

    void resizeViewport(uint32_t width, uint32_t height);
    void reloadShaders();

    void setGizmoOperation(ImGuizmo::OPERATION operation) { this->gizmoOperation = operation; };

private:
    void loadShaders();
    void setupImGui();

    SDL_Window *window = nullptr;
    SDL_GLContext context = nullptr;

    // gizmo
    ImGuizmo::OPERATION gizmoOperation = ImGuizmo::TRANSLATE;
    ImGuizmo::MODE gizmoMode = ImGuizmo::WORLD;
    Entity *selectedEntity = nullptr;

    Camera &camera;
    std::unordered_map<std::string, unsigned int> shaders;
};