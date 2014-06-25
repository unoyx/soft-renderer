#include "quaternion.h"

Quat operator*(const Quat &p, const Quat &q)
{
    // 从左向右顺序相乘， 与四元数标准定义不同。 [ w1w2 - v1*v2, w1 * v2 + w2 * v1 + v1 x v2 ]
    return Quat(p.w*q.w - p.x*q.x - p.y*q.y - p.z*q.z,
                p.w*q.x + p.x*q.w + p.y*q.z - p.z*q.y,
                p.w*q.y + p.y*q.w + p.z*q.x - p.x*q.z,
                p.w*q.z + p.z*q.w + p.x*q.y - p.y*q.x);
}

Vector3 RotateByQuat(const Vector3 &v, const Quat &q)
{
    Quat tmp(0, v);
    tmp = q * tmp * q.Inverse();
    return Vector3(tmp.x, tmp.y, tmp.z);
}

// TODO REMOVE
Quat RotateByQuat(const Quat &p, const Quat &q)
{
    Quat ret = q * p;

    return ret;
}