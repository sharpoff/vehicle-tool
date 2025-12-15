#pragma once

#include <filesystem>

#include "cgltf.h"
#include "types/model.h"

namespace gltf
{
    bool loadGltf(Model &model, std::filesystem::path path);
} // namespace gltf