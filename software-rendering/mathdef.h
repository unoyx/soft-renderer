#pragma once

#include <memory.h>
#include "typedef.h"
#include "vector.h"

static inline uint32 vector4_to_ARGB32(const Vector4 &c)
{
    uint8 a = (uint8)round(c.a * 255);
    uint8 r = (uint8)round(c.r * 255);
    uint8 g = (uint8)round(c.g * 255);
    uint8 b = (uint8)round(c.b * 255);
    uint32 ret = (a << 24) | (r << 16) | (g << 8) | (b << 0);
    return ret;
}

static inline Vector4 ARGB32_to_vector4(uint32 c)
{
    uint8 a = ((uint8)(c >> 24) & 0xFF);
    uint8 r = ((uint8)(c >> 16) & 0xFF);
    uint8 g = ((uint8)(c >> 8) & 0xFF);
    uint8 b = ((uint8)(c >> 0) & 0xFF);
    Vector4 ret(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
    return ret;
}

static inline uint32 ARGB32(uint8 a, uint8 r, uint8 g, uint8 b)
{
    uint32 ret = (a << 24) | (r << 16) | (g << 8) | (b << 0);
    return ret;
}

static inline Vector4 clamp(const Vector4 &v, const float min, const float max)
{
    clamp(v.x, min, max);
    clamp(v.y, min, max);
    clamp(v.z, min, max);
    clamp(v.w, min, max);
}