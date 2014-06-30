#pragma once
#include "vector.h"

class Light
{
public:
    Vector3 position;
    Vector4 diffuse;
    Vector4 specular;
    Vector4 ambient;
    float attenuation0;
    float attenuation1;
    float attenuation2;

    Light(void): attenuation0(0.0f), attenuation1(0.0f), attenuation2(0.0f) {}
    ~Light(void) {}
};

