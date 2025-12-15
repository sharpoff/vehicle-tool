#include "renderer.h"

#include <stdio.h>

#include "gl_utility.h"
#include "math_common.h"
#include "render_flags.h"
#include "types/entity.h"
#include "types/light.h"

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"
#include "ImGuizmo.h"

#include <glad/gl.h>

static void msgCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param)
{
	auto const src_str = [source]() {
		switch (source)
		{
            case GL_DEBUG_SOURCE_API: return "API";
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW SYSTEM";
            case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
            case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD PARTY";
            case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
            case GL_DEBUG_SOURCE_OTHER: return "OTHER";
            default: return "UNDEFINED";
		}
	}();

	auto const type_str = [type]() {
		switch (type)
		{
            case GL_DEBUG_TYPE_ERROR: return "ERROR";
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
            case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
            case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
            case GL_DEBUG_TYPE_MARKER: return "MARKER";
            case GL_DEBUG_TYPE_OTHER: return "OTHER";
            default: return "UNDEFINED";
		}
	}();

	auto const severity_str = [severity]() {
		switch (severity) {
            case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
            case GL_DEBUG_SEVERITY_LOW: return "LOW";
            case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
            case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
            default: return "UNDEFINED";
		}
	}();

    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) // skip msg with NOTIFICATION severity
        return;

    printf("%s, %s, %s, %d: %s\n", src_str, type_str, severity_str, id, message);
}

Renderer::Renderer(SDL_Window *window, Camera &camera)
    : window(window), camera(camera)
{
    context = SDL_GL_CreateContext(window);
    if (!context) {
        printf("Failed to create OpenGL context. %s\n", SDL_GetError());
        exit(-1);
    }

    SDL_GL_MakeCurrent(window, context);

    int version = gladLoadGL(SDL_GL_GetProcAddress);
    if (version == 0) {
        printf("Failed to initialize glad.\n");
        exit(-1);
    }

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(msgCallback, nullptr);

    uint32_t width = 0, height = 0;
    SDL_GetWindowSize(window, (int*)&width, (int*)&height);

    loadShaders();

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);

    setupImGui();
}

Renderer::~Renderer()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DestroyContext(context);
}

void Renderer::renderUI(Scene &scene)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    ImGuizmo::BeginFrame();

    if (selectedEntity) {
        ImGuiIO &io = ImGui::GetIO();
        ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
        float *matrix = glm::value_ptr(selectedEntity->getWorldMatrix());
        ImGuizmo::Manipulate(glm::value_ptr(camera.view), glm::value_ptr(camera.projection), gizmoOperation, gizmoMode, matrix, NULL, NULL);
    }

    {
        ImGui::Begin("Scene");

        std::vector<Entity*> &entities = scene.getEntities();

        for (size_t i = 0; i < entities.size(); i++) {
            Entity *entity = entities[i];
            bool selected = entity == selectedEntity;
            if (ImGui::Selectable(entity->getName().c_str(), selected)) {
                if (!selected) {
                    selectedEntity = entity;
                } else {
                    selectedEntity = nullptr;
                }
            }
        }

        ImGui::End();
    }

    ImGui::ShowDemoWindow();

    ImGui::Render();
    ImGuizmo::SetDrawlist(ImGui::GetBackgroundDrawList());
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Renderer::renderScene(Scene &scene)
{
    std::vector<Entity*> lights = scene.getEntitiesByType(EntityType::Light);

    glUseProgram(shaders["mesh"]);
    gl::setUniformUint(shaders["mesh"], lights.size(), "lightCount");

    int i = 0;
    for (Entity *entity : lights) {
        Light *light = static_cast<Light*>(entity);

        std::string lightIdx = "lights[" + std::to_string(i) + "]";
        gl::setUniformVec3(shaders["mesh"], math::getPosition(light->getWorldMatrix()), lightIdx + ".position");
        gl::setUniformVec3(shaders["mesh"], light->color, lightIdx + ".color");
        gl::setUniformUint(shaders["mesh"], (unsigned int)light->type, lightIdx + ".type");
        i++;
    }

    for (Entity *entity : scene.getEntities()) {
        if (entity->isRenderable())
            renderModel(entity->getModel(), RENDER_ALL, entity->getWorldMatrix());
    }
}

void Renderer::renderModel(Model &model, RenderFlags renderFlags, mat4 worldMatrix)
{
    glUseProgram(shaders["mesh"]);
    mat4 viewProjection = camera.projection * camera.view;
    gl::setUniformMat4(shaders["mesh"], viewProjection, "viewProjection");

    gl::setUniformUint(shaders["mesh"], (unsigned int)renderFlags, "renderFlags");

    for (Mesh &mesh : model.getMeshes()) {
        if (renderFlags & RENDER_BASE_COLOR_TEXTURE) {
            glBindTextureUnit(0, mesh.material.baseColorTexture);
            gl::setUniformInt(shaders["mesh"], 0, "material.baseColorTexture");
        }

        if (renderFlags & RENDER_COLOR) {
            gl::setUniformVec3(shaders["mesh"], mesh.material.baseColor, "material.baseColor");
        }

        mat4 world = worldMatrix * mesh.getWorldMatrix();
        gl::setUniformMat4(shaders["mesh"], world, "world");

        mat4 invWorld = glm::inverse(world);
        gl::setUniformMat4(shaders["mesh"], invWorld, "invWorld");

        if (!mesh.indices.empty()) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.indexBuffer);
            glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
        } else {
            glBindVertexArray(mesh.vertexArray);
            glDrawArrays(GL_TRIANGLES, 0, mesh.vertices.size());
        }
    }
}

void Renderer::resizeViewport(uint32_t width, uint32_t height)
{
    glViewport(0, 0, width, height);
}

void Renderer::reloadShaders()
{
    loadShaders();
}

void Renderer::loadShaders()
{
    shaders["mesh"] = gl::createProgram("shaders/mesh.vert", "shaders/mesh.frag");
}

void Renderer::setupImGui()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

    float displayContentScale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());

    // Setup scaling
    ImGuiStyle &style = ImGui::GetStyle();
    style.ScaleAllSizes(displayContentScale); // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = displayContentScale; // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForOpenGL(window, context);
    ImGui_ImplOpenGL3_Init("#version 460");
}