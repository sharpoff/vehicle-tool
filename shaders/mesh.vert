#version 460 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;

out vec2 outUV;
out vec3 outNormal;
out vec3 outWorldPos;

uniform mat4 viewProjection;
uniform mat4 world;
uniform mat4 invWorld;

void main()
{
    vec4 worldPos = world * vec4(pos, 1.0);
    gl_Position = viewProjection * worldPos;
    outNormal = normalize(mat3(transpose(invWorld)) * normal);
    outUV = uv;
    outWorldPos = vec3(worldPos);
}