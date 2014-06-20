#pragma once

#include <math.h>
#include <memory.h>

#ifndef WIN64
typedef unsigned int uint32;
typedef int int32;
#endif
typedef unsigned char uint8;

static const float gkPi = 3.141592653f;

static const float gkFloatPrecise = 0.0001f;

// radian angle
static inline float angle2radian(float angle)
{
    return gkPi * angle / 180.0f;
}

static inline float radian2angle(float radian)
{
    return 180.0f * radian / gkPi;
}

static inline void sincosf(float angle, float *s, float *c)
{
    *s = sinf(angle);
    *c = cosf(angle);
}

template<typename T>
T min_t(const T &x, const T &y)
{
    return ((x < y) ? x : y);
}

template<typename T>
T max_t(const T &x, const T &y)
{
    return ((x < y) ? y : x);
}

template<typename T>
inline void swap(T& a, T& b)
{
    T c = a;
    a = b;
    b = c;
}

inline int round(float f)
{
    return static_cast<int>(f + 0.5f);
}

inline float absf(float x)
{
    if (x > 0.0f)
        return x;
    else
        return -x;
}

inline bool equalf(float a, float b)
{
    return (absf(a - b) < gkFloatPrecise);
}
