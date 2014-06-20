#pragma once
#include "vector.h"
#include "quaternion.h"

class Camera
{
public:
    Camera(void)
        :far_(0.0f)
        ,near_(0.0f)
        ,fov_(0.0f) {}

    Camera(const Vector3 &pos, const Quat &ori, float far, float near, float fov, float aspect)
        :pos_(pos)
        ,ori_(ori)
        ,far_(far)
        ,near_(near)
        ,fov_(fov) {}

    ~Camera(void) {}
    
    void Rotate(float heading, float pitch);
    void Move(const Vector3 &v);

private:
    Vector3 pos_;
    Quat ori_;

    float far_;
    float near_;
    // yz Æ½Ãæ
    float fov_;
};

