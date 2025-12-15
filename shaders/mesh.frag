#version 460 core

// LightType
const uint LIGHT_DIRECTIONAL = 0;
const uint LIGHT_POINT = 1;
const uint LIGHT_SPOT = 2;

// RenderFlags
const uint RENDER_LIGHT = 1;
const uint RENDER_COLOR = 1 << 1;
const uint RENDER_BASE_COLOR_TEXTURE = 1 << 2;
const uint RENDER_ALL = RENDER_LIGHT | RENDER_COLOR | RENDER_BASE_COLOR_TEXTURE;

const uint MAX_LIGHTS = 5;

struct Light
{
    vec3 position;
    vec3 color;
    uint type;
};

struct Material
{
    sampler2D baseColorTexture;
    vec3 baseColor;
};

in vec2 outUV;
in vec3 outNormal;
in vec3 outWorldPos;

out vec4 fragColor;

uniform Light lights[MAX_LIGHTS];
uniform uint lightCount;
uniform Material material;
uniform uint renderFlags;

vec3 linearToSRGB(vec3 rgb)
{
    return mix(1.055 * pow(rgb, vec3(1.0 / 2.4)) - 0.055, rgb * 12.92, lessThanEqual(rgb, vec3(0.0031308)));
}

#define isBitSet(flag, bit) ((flag) & (bit)) == (bit)

void main()
{
    vec3 color = vec3(1.0);

    if (isBitSet(renderFlags, RENDER_BASE_COLOR_TEXTURE)) {
        vec4 tex = texture(material.baseColorTexture, outUV);
        if (tex.a < 0.5)
            discard;
        
        color *= tex.rgb;
    }

    if (isBitSet(renderFlags, RENDER_COLOR)) {
        color *= material.baseColor;
    }

    if (isBitSet(renderFlags, RENDER_LIGHT)) {
        float diffuse = 0.0f;
        for (uint i = 0; i < lightCount; i++) {
            vec3 lightDir = normalize(lights[i].position - outWorldPos);
            diffuse += max(dot(outNormal, lightDir), 0.0);
        }
        color *= diffuse;

        // float ambient = 0.01;
        // color *= ambient;
    }

    // vec3 color = vec3(1.0, 0.0, 0.0);

    fragColor = vec4(linearToSRGB(color), 1.0);
}