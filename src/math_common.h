#pragma once

#include "math_defines.h"
#include <glm/gtx/matrix_decompose.hpp>

namespace math
{
    inline vec3 getPosition(mat4 matrix)
    {
        vec3 scale;
        quat orientation;
        vec3 translation;
        vec3 skew;
        vec4 perspective;

        glm::decompose(matrix, scale, orientation, translation, skew, perspective);
        return translation;
    }

    inline quat getRotation(mat4 matrix)
    {
        vec3 scale;
        quat orientation;
        vec3 translation;
        vec3 skew;
        vec4 perspective;

        glm::decompose(matrix, scale, orientation, translation, skew, perspective);
        return orientation;
    }

    inline quat getScale(mat4 matrix)
    {
        vec3 scale;
        quat orientation;
        vec3 translation;
        vec3 skew;
        vec4 perspective;

        glm::decompose(matrix, scale, orientation, translation, skew, perspective);
        return scale;
    }
}