#pragma once

#include "math_defines.h"
#include "types/entity.h"

enum class LightType
{
    Directional = 0,
    Point,
    Spot,
};

struct Light : Entity
{
    Light(std::string name) : Entity(EntityType::Light, name) {}

    vec3 color = vec3(1.0f);
    LightType type = LightType::Directional;
};