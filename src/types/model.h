#pragma once

#include "types/mesh.h"

class Model
{
public:
    void setupBuffers();

    void addMesh(Mesh &mesh);
    void setCustomMaterial(const Material &material);

    std::vector<Mesh> &getMeshes() { return meshes; };

private:
    std::vector<Mesh> meshes;
};