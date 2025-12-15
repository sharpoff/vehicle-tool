#pragma once

#include <vector>
#include <filesystem>

namespace utility
{
    std::vector<char> loadFile(std::filesystem::path path);
}