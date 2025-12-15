#pragma once

#include <filesystem>

#include "types/model.h"

namespace obj
{
    bool loadObj(Model &model, std::filesystem::path path);
}