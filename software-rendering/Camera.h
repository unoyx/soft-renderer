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

    Camera(const Vector3 &pos, const Quat &ori, float z_far, float near, float fov)
        :pos_(pos)
        ,ori_(ori)
        ,far_(z_far)
        ,near_(near)
        ,fov_(fov) {}

// TODO
//    Camera(const Vector3 &pos, const Vector3 &look_at, float far, float near, float fov, float aspect)
//    {}

    ~Camera(void) {}
    
    void Rotate(float heading, float pitch);
    void Move(const Vector3 &v);

    Matrix44 GetPerpectivMatrix(void);
    Matrix44 GetModelViewMatrix(void);

private:
    Vector3 pos_;
    Quat ori_;

    float far_;
    float near_;
    // yz 平面的fov角, 角度表示
    float fov_;

    float aspect_;
};

