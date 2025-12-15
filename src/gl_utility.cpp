#include "gl_utility.h"
#include "glm/gtc/type_ptr.hpp"
#include "utility.h"

#include <GLES2/gl2.h>
#include <glad/gl.h>
#include <stb_image.h>

namespace gl
{
    unsigned int createProgram(std::string vertexShaderPath, std::string fragmentShaderPath)
    {
        std::vector<char> vsCode = utility::loadFile(vertexShaderPath);
        std::vector<char> fsCode = utility::loadFile(fragmentShaderPath);

        std::string vsStr = std::string(vsCode.begin(), vsCode.end());
        std::string fsStr = std::string(fsCode.begin(), fsCode.end());

        const char *vsCStr = vsStr.c_str();
        const char *fsCStr = fsStr.c_str();

        // debug stuff
        int success;
        char infoLog[512];

        // compile vertex
        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, &vsCStr, NULL);
        glCompileShader(vs);
        glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vs, 512, NULL, infoLog);
            printf("Failed to compile vertex shader - %s\n", infoLog);
        };

        // compile fragment
        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, &fsCStr, NULL);
        glCompileShader(fs);
        glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fs, 512, NULL, infoLog);
            printf("Failed to compile fragment shader - %s\n", infoLog);
        };

        GLuint program = glCreateProgram();

        // link shaders
        glAttachShader(program, vs);
        glAttachShader(program, fs);
        glLinkProgram(program);
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(fs, 512, NULL, infoLog);
            printf("Failed to link program - %s\n", infoLog);
        };

        glDeleteShader(vs);
        glDeleteShader(fs);

        return program;
    }

    unsigned int createTexture(void *data, uint32_t width, uint32_t height, bool rgba)
    {
        unsigned int id;
        glCreateTextures(GL_TEXTURE_2D, 1, &id);

        glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        GLenum internalFormat = rgba ? GL_RGBA8 : GL_RGB8;
        GLenum format = rgba ? GL_RGBA : GL_RGB;

        glTextureStorage2D(id, 1, internalFormat, width, height);
        glTextureSubImage2D(id, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, data);
        return id;
    }

    unsigned int createTextureFromFile(std::filesystem::path filename)
    {
        stbi_set_flip_vertically_on_load(true);

        uint32_t width, height, channels;
        unsigned char *pixels = stbi_load(filename.c_str(), (int*)&width, (int*)&height, (int*)&channels, STBI_rgb_alpha);
        if (!pixels) {
            printf("Failed to create texture from file '%s'!\n", filename.c_str());
            return 0;
        }

        return createTexture(pixels, width, height, true);
    }

    void setUniformInt(unsigned int shader, int integer, std::string name)
    {
        GLint loc = glGetUniformLocation(shader, name.c_str());
        glUniform1i(loc, integer);
    }

    void setUniformUint(unsigned int shader, unsigned int integer, std::string name)
    {
        GLint loc = glGetUniformLocation(shader, name.c_str());
        glUniform1ui(loc, integer);
    }

    void setUniformVec2(unsigned int shader, glm::vec2 vec, std::string name)
    {
        GLint loc = glGetUniformLocation(shader, name.c_str());
        glUniform2f(loc, vec.x, vec.y);
    }

    void setUniformVec3(unsigned int shader, glm::vec3 vec, std::string name)
    {
        GLint loc = glGetUniformLocation(shader, name.c_str());
        glUniform3f(loc, vec.x, vec.y, vec.z);
    }

    void setUniformMat4(unsigned int shader, glm::mat4 mat, std::string name)
    {
        GLint loc = glGetUniformLocation(shader, name.c_str());
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
    }
} // namespace gl