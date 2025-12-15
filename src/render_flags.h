#pragma once

#include <stdint.h>

enum RenderFlags : uint8_t
{
    RENDER_LIGHT = 1,
    RENDER_COLOR = 1 << 1,
    RENDER_BASE_COLOR_TEXTURE = 1 << 2,
    RENDER_ALL = RENDER_LIGHT | RENDER_COLOR | RENDER_BASE_COLOR_TEXTURE
};