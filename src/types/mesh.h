#pragma once

#include "math_defines.h"
#include "types/material.h"
#include "types/vertex.h"

class Mesh
{
public:
    mat4 getWorldMatrix() { return worldMatrix; }

    Material material;

    std::vector<Vertex>   vertices;
    std::vector<uint32_t> indices;

    unsigned int vertexArray;
    unsigned int vertexBuffer;
    unsigned int indexBuffer;

private:
    mat4 worldMatrix = mat4(1.0f);
};
