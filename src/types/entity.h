#pragma once

#include "model.h"
#include <limits>
#include <string>

enum class EntityId : uint32_t { Invalid = std::numeric_limits<uint32_t>::max() };

enum class EntityType
{
    Model,
    Light
};

class Entity
{
public:
    Entity(EntityType type, std::string name = "") : type(type), name(name) { }

    void setName(std::string name) { this->name = name; }
    void setModel(Model model) { this->model = model; };
    void setWorldMatrix(mat4 matrix) { this->worldMatrix = matrix; };
    void setIsRenderable(bool renderable) {this->renderable = renderable; };

    std::string &getName() { return name; }
    Model &getModel() { return model; }
    mat4 &getWorldMatrix() { return worldMatrix; };
    EntityType &getEntityType() { return type; }

    bool isRenderable() { return renderable; }

private:
    EntityType type = EntityType::Model;

    Model model;
    mat4 worldMatrix = mat4(1.0f);
    std::string name = "";

    bool renderable = true;
};