#pragma once

#include <stdint.h>
#include <string>
#include <filesystem>

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/matrix.hpp>

namespace gl
{
    unsigned int createProgram(std::string vertexShaderPath, std::string fragmentShaderPath);

    unsigned int createTexture(void *data, uint32_t width, uint32_t height, bool rgba = true);
    unsigned int createTextureFromFile(std::filesystem::path filename);

    void setUniformInt(unsigned int shader, int integer, std::string name);
    void setUniformUint(unsigned int shader, unsigned int integer, std::string name);
    void setUniformVec2(unsigned int shader, glm::vec2 vec, std::string name);
    void setUniformVec3(unsigned int shader, glm::vec3 vec, std::string name);
    void setUniformMat4(unsigned int shader, glm::mat4 mat, std::string name);
} // namespace gl