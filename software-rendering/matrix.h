#pragma once
#include "mathdef.h"
#include "vector.h"
#ifdef _DEBUG
#include "Logger.h"
#endif

#pragma warning(push)
// 禁用关于匿名结构的警告
#pragma warning(disable:4201)

class Matrix33
{
private:
    enum 
    {
        ELE_SIZE = 3,
        MEM_SIZE = 9
    };
public:
    union
    {
        // FIXME 考虑字节对齐的问题
        float e[ELE_SIZE][ELE_SIZE];
        float m[MEM_SIZE];
        struct {float m00; float m01; float m02;
        float m10; float m11; float m12;
        float m20; float m21; float m22;};
    };

    Matrix33(void) { memset(m, 0, sizeof(m)); }
    Matrix33(const Matrix33& o)
    {
        memcpy(m, o.m, sizeof(m));
    }
    Matrix33& operator=(const Matrix33& o)
    {
        memcpy(m, o.m, sizeof(m));
        return *this;
    }
    explicit Matrix33(const float* p)
    {
        memcpy(m, p, sizeof(m));
    }
    Matrix33(float f00, float f01, float f02,
        float f10, float f11, float f12,
        float f20, float f21, float f22)
    {
        m00 = f00; m01 = f01; m02 = f02;
        m10 = f10; m11 = f11; m12 = f12;
        m20 = f20; m21 = f21; m22 = f22;
    }
    Matrix33(const Vector3& v1, const Vector3& v2, const Vector3& v3)
    {
        m00 = v1.x; m01 = v1.y; m02 = v1.z;
        m10 = v2.x; m11 = v2.y; m12 = v2.z;
        m20 = v3.x; m21 = v3.y; m22 = v3.z;
    }

    Matrix33 operator+(const Matrix33& o)
    {
        for (int i = 0; i < MEM_SIZE; ++i)
        {
            m[i] += o.m[i];
        }
    }

    Matrix33 operator-(const Matrix33& o)
    {
        for (int i = 0; i < MEM_SIZE; ++i)
        {
            m[i] -= o.m[i];
        }
    }

    Matrix33 operator*(const Matrix33& o)
    {
        Matrix33 res;
        for (int i = 0; i < ELE_SIZE; ++i)
        {
            for (int j = 0; j < ELE_SIZE; ++j)
            {
                for (int k = 0; k < ELE_SIZE; ++k)
                {
                    res.e[i][j] += e[i][k] * o.e[k][j];
                }
            }
        }
        return res;
    }

    Vector3 operator*(const Vector3& v)
    {
        Vector3 res;
        for (int i = 0; i < 3; ++i)
        {
            for (int k = 0; k < ELE_SIZE; ++k)
            {
                res.m[i] += e[i][k] * v.m[k];
            }
        }
        return res;
    }

    Matrix33 operator*(float s)
    {
        Matrix33 res;
        for (int i = 0; i < MEM_SIZE; ++i)
        {
            res.m[i] = m[i] * s;
        }
        return res;
    }

    void Transpose(void)
    {
        swap(m10, m01);
        swap(m20, m02);
        swap(m21, m12);
    }

    void Clear(void)
    {
        memset(m, 0, sizeof(m));
    }

    void SetIdentity(void)
    {
        Clear();
        m00 = m11 = m22 = 1.0f;
    }

    static Matrix33 CreateIdentity(void)
    {
        Matrix33 ret;
        ret.SetIdentity();
        return ret;
    }

#ifdef _DEBUG
    void Display(void)
    {
        Logger::GtLogInfo("Matrix33 content:");
        Logger::GtLog("%8.3f %8.3f %8.3f", m00, m01, m02);
        Logger::GtLog("%8.3f %8.3f %8.3f", m10, m11, m12);
        Logger::GtLog("%8.3f %8.3f %8.3f", m20, m21, m22);
        return;
    }
#endif
};

class Matrix44
{
private:
    enum
    {
        ELE_SIZE = 4,
        MEM_SIZE = 16
    };
public:
    union
    {
        // FIXME 考虑字节对齐的问题
        float e[ELE_SIZE][ELE_SIZE];
        float m[MEM_SIZE];
        struct {
            float m00; float m01; float m02; float m03;
            float m10; float m11; float m12; float m13;
            float m20; float m21; float m22; float m23;
            float m30; float m31; float m32; float m33;
        };
    };

    Matrix44(void) { memset(m, 0, sizeof(m)); }
    Matrix44(const Matrix44& o)
    {
        memcpy(m, o.m, sizeof(m));
    }
    Matrix44& operator=(const Matrix44& o)
    {
        memcpy(m, o.m, sizeof(m));
        return *this;
    }
    explicit Matrix44(const float* p)
    {
        memcpy(m, p, sizeof(m));
    }
    Matrix44(float f00, float f01, float f02, float f03,
             float f10, float f11, float f12, float f13,
             float f20, float f21, float f22, float f23,
             float f30, float f31, float f32, float f33)
    {
        m00 = f00; m01 = f01; m02 = f02; m03 = f03;
        m10 = f10; m11 = f11; m12 = f12; m13 = f13;
        m20 = f20; m21 = f21; m22 = f22; m23 = f23;
        m30 = f30; m31 = f31; m32 = f32; m33 = f33;
    }

    Matrix44(const Vector3& v1, const Vector3& v2, const Vector3& v3)
    {
        m00 = v1.x; m01 = v1.y; m02 = v1.z; m03 = 0;
        m10 = v2.x; m11 = v2.y; m12 = v2.z; m13 = 0;
        m20 = v3.x; m21 = v3.y; m22 = v3.z; m23 = 0;
        m30 = 0;    m31 = 0;    m32 = 0;    m33 = 0;
    }

    Matrix44 operator+(const Matrix44& o)
    {
        for (int i = 0; i < MEM_SIZE; ++i)
        {
            m[i] += o.m[i];
        }
    }

    Matrix44 operator-(const Matrix44& o)
    {
        for (int i = 0; i < MEM_SIZE; ++i)
        {
            m[i] -= o.m[i];
        }
    }

    Matrix44 operator*(const Matrix44& o)
    {
        Matrix44 res;
        for (int i = 0; i < ELE_SIZE; ++i)
        {
            for (int j = 0; j < ELE_SIZE; ++j)
            {
                for (int k = 0; k < ELE_SIZE; ++k)
                {
                    res.e[i][j] += e[i][k] * o.e[k][j];
                }
            }
        }
        return res;
    }

    //Vector3 MulVector3(const Vector3& v)
    //{
    //    Vector3 res;
    //    for (int i = 0; i < ELE_SIZE; ++i)
    //    {
    //        for (int k = 0; k < ELE_SIZE; ++k)
    //        {
    //            res.m[i] += e[i][k] * v.m[k];
    //        }
    //    }
    //    return res;
    //}

    Matrix44 operator*(float s)
    {
        Matrix44 res;
        for (int i = 0; i < MEM_SIZE; ++i)
        {
            res.m[i] = m[i] * s;
        }
        return res;
    }

    void Transpose(void)
    {
        swap(m10, m01);
        swap(m20, m02);
        swap(m21, m12);
        swap(m30, m03);
        swap(m31, m13);
        swap(m32, m23);
    }

    void Clear(void)
    {
        memset(m, 0, sizeof(m));
    }

    void SetIdentity(void)
    {
        Clear();
        m00 = m11 = m22 = m33 = 1.0f;
    }
    
    static Matrix44 CreateIdentity(void)
    {
        Matrix44 ret;
        ret.SetIdentity();
        return ret;
    }

#ifdef _DEBUG
    void Display(void)
    {
        Logger::GtLogInfo("Matrix44 content:");
        Logger::GtLog("%8.3f %8.3f %8.3f %8.3f", m00, m01, m02, m03);
        Logger::GtLog("%8.3f %8.3f %8.3f %8.3f", m10, m11, m12, m13);
        Logger::GtLog("%8.3f %8.3f %8.3f %8.3f", m20, m21, m22, m23);
        Logger::GtLog("%8.3f %8.3f %8.3f %8.3f", m30, m31, m32, m33);
        return;
    }
#endif
};

Vector3 operator*(const Vector3 &v, const Matrix44 &m)
{
    Vector3 ret;
    ret.x = v.x * m.m00 + v.y * m.m10 + v.z * m.m20 + m.m30;
    ret.y = v.x * m.m01 + v.y * m.m11 + v.z * m.m21 + m.m31;
    ret.z = v.x * m.m02 + v.y * m.m12 + v.z * m.m22 + m.m32;
    return ret;
}

#pragma warning(default:4201)
//  启用匿名结构的警告
#pragma warning(pop)