#pragma once
#include "vector.h"
#include "quaternion.h"

class Camera
{
public:
    Camera(void)
        :pos_(0, 0, 0)
        ,ori_(Quat::GetIdentity())
        ,far_(0)
        ,near_(0)
        ,fov_(0)
        ,aspect_(0) {}
    Camera(const Vector3 &pos, const Quat &ori, float z_far, float z_near, float fov, float aspect)
        :pos_(pos)
        ,ori_(ori)
        ,far_(z_far)
        ,near_(z_near)
        ,fov_(fov)
        ,aspect_(aspect){}
// TODO
//    Camera(const Vector3 &pos, const Vector3 &look_at, float far, float near, float fov, float aspect)
//    {}
    ~Camera(void) {}

    void set_pos(const Vector3 &pos)
    {
        pos_ = pos;
    }

    void set_ori(const Quat &ori)
    {
        ori_ = ori;
    }

    void set_far(float f)
    {
        far_ = f;
    }

    float get_far(void)
    {
        return far_;
    }

    void set_near(float n)
    {
        near_ = n;
    }

    float get_near(void)
    {
        return near_;
    }

    void set_fov(float fov)
    {
        fov_ = fov;
    }

    void set_aspect(float aspect)
    {
        aspect_ = aspect;
    }
    
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

