#include "Camera.h"

void Camera::Rotate(float heading, float pitch)
{
    Quat rotate = Quat::GetRotationY(heading);
    ori_ = RotateByQuat(ori_, rotate);
    rotate.SetRotationX(pitch);
    ori_ = RotateByQuat(ori_, rotate);
}

void Camera::Move(const Vector3 &v)
{
    pos_ += v;
}

// 获取透视矩阵
Matrix44 Camera::GetPerpectivMatrix(void)
{
    float rad_fov = angle2radian(fov_);
    Matrix44 m;
    m.SetPerspectiveMatrixLH(far_, near_, rad_fov, aspect_);
    return m;
}

// 获取旋转平移矩阵
Matrix44 Camera::GetModelViewMatrix(void)
{
    // fixme 旋转方向不正常
    Matrix33 rotate = ori_.Inverse().GetMatrix33();
    Matrix44 m(rotate);
    m.SetTranslation(pos_.x, pos_.y, pos_.z);
    return m;
}


