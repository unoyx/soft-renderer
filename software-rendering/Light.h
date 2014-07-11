#pragma once
#include "vector.h"

class Light
{
public:
    Light(void): attenuation0(0.0f), attenuation1(0.0f), attenuation2(0.0f) {}
    ~Light(void) {}

    void set_position(float x, float y, float z)
    {
        position = Vector3(x, y, z);
    }

    void set_diffuse(float r, float g, float b)
    {
        diffuse = Vector4(r, g, b, 1.0f);
    }

    void set_ambient(float r, float g, float b)
    {
        ambient = Vector4(r, g, b, 1.0f);
    }

    void set_specular(float r, float g, float b, float w)
    {
        specular = Vector4(r, g, b, w);
    }

    Vector3 position;
    Vector4 diffuse;
    Vector4 specular;
    Vector4 ambient;
    float attenuation0;
    float attenuation1;
    float attenuation2;
};

