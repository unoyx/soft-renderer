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

// ��ȡ͸�Ӿ���
Matrix44 Camera::GetPerpectivMatrix(void)
{
    float rad_fov = angle2radian(fov_);
    Matrix44 m;
    m.SetPerspectiveMatrixLH(far_, near_, rad_fov, aspect_);
    return m;
}

// ��ȡ��תƽ�ƾ���
Matrix44 Camera::GetModelViewMatrix(void)
{
    // fixme ��ת��������
    Matrix33 rotate = ori_.Inverse().GetMatrix33();
    Matrix44 m(rotate);
    m.SetTranslation(pos_.x, pos_.y, pos_.z);
    return m;
}


