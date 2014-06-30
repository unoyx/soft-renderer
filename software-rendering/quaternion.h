#pragma once
#include "typedef.h"
#include "vector.h"
#include "matrix.h"
#ifdef _DEBUG
#include "assert.h"
#include "Logger.h"
#endif

#pragma warning(push)
// 禁用关于匿名结构的警告
#pragma warning(disable:4201)

class Quat
{
public:
    union 
    {
        struct 
        {
            float w; float x; float y; float z;
        };
        struct 
        {
            float w; Vector3 v;
        };
    };
public:
    Quat(void) {}
    Quat(float W, float X, float Y, float Z)
        : w(W)
        , v(X, Y, Z) {}
    
    Quat(float W, const Vector3 &V)
        : w(W)
        , v(V) {}

    ~Quat(void) {}

    void SetIdentity(void)
    {
        w = 1; x = 0; y = 0; z = 0;
    }
    static Quat GetIdentity(void)
    {
        Quat ret;
        ret.SetIdentity();
        return ret;
    }

    void SetRotationX(float x_angle)
    {
        float rad = angle2radian(x_angle);
        sincosf(rad * 0.5f, &x, &w);
        y = 0;
        z = 0;
    }
    static Quat GetRotationX(float x_angle)
    {
        Quat ret;
        ret.SetRotationX(x_angle);
        return ret;
    }

    void SetRotationY(float y_angle)
    {
        float rad = angle2radian(y_angle);
        sincosf(rad * 0.5f, &y, &w);
        x = 0;
        z = 0;
    }
    static Quat GetRotationY(float y_angle)
    {
        Quat ret;
        ret.SetRotationY(y_angle);
        return ret;
    }

    void SetRotationZ(float z_angle)
    {
        float rad = angle2radian(z_angle);
        sincosf(rad * 0.5f, &z, &w);
        x = 0;
        y = 0;
    }
    static Quat GetRotationZ(float z_angle)
    {
        Quat ret;
        ret.SetRotationZ(z_angle);
        return ret;
    }

    void SetRotationAxis(const Vector3 &axis, float angle)
    {
        float rad = angle2radian(angle);
        float s = 0.0f;
        sincosf(rad * 0.5f, &s, &w);
        v = s * axis;
    }

    // 转化四元数为3x3矩阵
    Matrix33 GetMatrix33(void) const
    {
        Matrix33 ret(GetRow0(),
                     GetRow1(),
                     GetRow2());
        return ret;
    }

    Vector3 GetColumn0(void) const 
    {
        Vector3 ret(1 - 2*y*y - 2*z*z,
                    2*x*y - 2*w*z,
                    2*x*z + 2*w*y);
        return ret;
    }

    Vector3 GetColumn1(void) const 
    {
        Vector3 ret(2*x*y + 2*w*z,
                    1 - 2*x*x - 2*z*z,
                    2*y*z - 2*w*x);
    }
    Vector3 GetColumn2(void) const 
    {
        Vector3 ret(2*x*z - 2*w*y,
                    2*y*z + 2*w*x,
                    1 - 2*x*x - 2*y*y);
        return ret;
    }

    Vector3 GetRow0(void) const 
    {
        Vector3 ret(1 - 2*y*y - 2*z*z,
                    2*x*y + 2*w*z,
                    2*x*z - 2*w*y);
        return ret;
    }

    Vector3 GetRow1(void) const 
    {
        Vector3 ret(2*x*y - 2*w*z,
                    1 - 2*x*x - 2*z*z,
                    2*y*z + 2*w*x);
        return ret;
    }

    Vector3 GetRow2(void) const 
    {
        Vector3 ret(2*x*z + 2*w*y,
                    2*y*z - 2*w*x,
                    1 - 2*x*x - 2*y*y);
        return ret;
    }

    Quat operator-(void) const 
    {
        return Quat(-w, -v);
    }

    float Magnitude(void) const 
    {
        return sqrtf(w * w + x * x + y * y + z * z);
    }

    // 共轭
    Quat Conjugate(void) const 
    {
        return Quat(w, -v);
    }

    // 四元数的逆
    Quat Inverse(void) const
    {
        float mag = Magnitude();
        if (equalf(mag, 1.0f))
        {
            return Conjugate();
        }
        else
        {
            float one_over_mag = 1.0f / mag;
            return Quat(w * one_over_mag,
                        -x * one_over_mag,
                        -y * one_over_mag,
                        -z * one_over_mag);
        }
    }

    Quat operator*(const Quat &q)
    {
        // 从左向右顺序相乘， 与四元数标准定义不同。 [ w1w2 - v1*v2, w1 * v2 + w2 * v1 + v1 x v2 ]
        return Quat(w*q.w - x*q.x - y*q.y - z*q.z,
                    w*q.x + x*q.w + y*q.z - z*q.y,
                    w*q.y + y*q.w + z*q.x - x*q.z,
                    w*q.z + z*q.w + x*q.y - y*q.x);
    }

    bool IsUnit(float precise = gkFloatPrecise)
    {
        float mag = Magnitude();
        return abs(mag - 1.0f) < precise;
    }

    void Normalize(void)
    {
        float mag = sqrtf(w * w + x * x + y * y + z * z);
        if (mag > 0.0f)
        {
            float one_over_mag = 1.0f / mag;
            w *= one_over_mag;
            x *= one_over_mag;
            y *= one_over_mag;
            z *= one_over_mag;
        }
        else
        {
            assert(0);
        }
    }
#ifdef _DEBUG
    void Display(void)
    {
        Logger::GtLogInfo("Quat content:");
        Logger::GtLog("%8.3f %8.3f %8.3f %8.3f", w, x, y, z);
        return;
    }
#endif
};

Quat operator*(const Quat &p, const Quat &q);

Vector3 RotateByQuat(const Vector3 &v, const Quat &q);

#pragma warning(default:4201)
//  启用匿名结构的警告
#pragma warning(pop)