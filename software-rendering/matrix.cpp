#include "matrix.h"

Vector3 operator*(const Vector3 &v, const Matrix33 &m)
{
    static const int ELE_SIZE = 3;
    Vector3 ret;
    for (int i = 0; i < ELE_SIZE; ++i)
    {
        for (int k = 0; k < ELE_SIZE; ++k)
        {
            ret.m[i] += v.m[k] * m.e[k][i];
        }
    }
    return ret;
}

Vector3 operator*(const Vector3 &v, const Matrix44 &m)
{
    Vector3 ret;
    ret.x = v.x * m.m00 + v.y * m.m10 + v.z * m.m20 + m.m30;
    ret.y = v.x * m.m01 + v.y * m.m11 + v.z * m.m21 + m.m31;
    ret.z = v.x * m.m02 + v.y * m.m12 + v.z * m.m22 + m.m32;
    return ret;
}

Vector4 operator*(const Vector4 &v, const Matrix44 &m)
{
    static const int ELE_SIZE = 4;
    Vector4 ret;
    for (int i = 0; i < ELE_SIZE; ++i)
    {
        for (int k = 0; k < ELE_SIZE; ++k)
        {
            ret.m[i] += v.m[k] * m.e[k][i];
        }
    }
    return ret;
}
