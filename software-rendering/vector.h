#pragma once
#include "mathdef.h"
#ifdef _DEBUG
#include "Logger.h"
#endif

#pragma warning(push)
// 禁用关于匿名结构的警告
#pragma warning(disable:4201)
class Point
{
public:
    int32 x;
    int32 y;

    Point() : x(0), y(0) {}
    Point(int x, int y) : x(x), y(y) {}
    Point(const Point& p) : x(p.x), y(p.y) {}
    bool operator==(const Point& p)
    {
        return (x == p.x && y == p.y);
    }
};

class Vector2
{
public:
    float x;
    float y;

    Vector2() : x(0.0f), y(0.0f) {}
    Vector2(float x, float y) : x(x), y(y) {}
    Vector2(const Vector2& v) : x(v.x), y(v.y) {}
    Vector2& operator=(const Vector2& v)
    {
        x = v.x;
        y = v.y;
        return *this;
    }

    void SetZero(void) 
    {
        x = 0.0f;
        y = 0.0f;
    }

    Vector2 operator-(void)
    {
        return Vector2(-x, -y);
    }

    Vector2& operator+=(const Vector2& v)
    {
        x += v.x;
        y += v.y;
        return *this;
    }

    Vector2& operator-=(const Vector2& v)
    {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    Vector2& operator*=(float s)
    {
        x *= s;
        y *= s;
        return *this;
    }

    Vector2& operator/=(float s)
    {
        x /= s;
        y /= s;
        return *this;
    }

    Vector2 operator+(const Vector2& v) const
    {
        return Vector2(x + v.x, y + v.y);
    }

    Vector2 operator-(const Vector2& v) const
    {
        return Vector2(x - v.x, y - v.y);
    }

    Vector2 operator*(float s) const
    {
        return Vector2(x * s, y * s);
    }

    Vector2 operator*(Vector2 v) const
    {
        return Vector2(x * v.x, y * v.y);
    }

    Vector2 operator/(float d) const
    {
        return Vector2(x / d, y / d);
    }

    void Normalize(void)
    {
        float mag_sq = x * x + y * y;
        if (mag_sq > 0)
        {
            float one_over_mag = 1.0f / sqrtf(mag_sq);
            x *= one_over_mag;
            y *= one_over_mag;
        }
    }
#ifdef _DEBUG
    void Display(void)
    {
        Logger::GtLogInfo("Vector2 content:");
        Logger::GtLog("%f8.3 %f8.3", x, y);
    }
#endif
};

inline Vector2 operator*(float s, const Vector2& v)
{
    return Vector2(s * v.x, s * v.y);
}

class Vector3
{
private:
    enum 
    {
        ELE_SIZE = 3
    };
public:
    union
    {
        struct 
        {
            float x;
            float y;
            float z;
        };
        float m[ELE_SIZE];
    };

    Vector3() : x(0.0f), y(0.0f), z(0.0f) {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
    Vector3(const Vector3& v) : x(v.x), y(v.y), z(v.z) {}
    Vector3& operator=(const Vector3& v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
        return *this;
    }

    void SetZero(void) 
    {
        x = 0.0f;
        y = 0.0f;
        z = 0.0f;
    }

    Vector3 operator-(void)
    {
        return Vector3(-x, -y, -z);
    }

    Vector3& operator+=(const Vector3& v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    Vector3& operator-=(const Vector3& v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }

    Vector3& operator*=(float s)
    {
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }

    Vector3& operator/=(float s)
    {
        x /= s;
        y /= s;
        z /= s;
        return *this;
    }

    Vector3 operator+(const Vector3& v) const
    {
        return Vector3(x + v.x, y + v.y, z + v.z);
    }

    Vector3 operator-(const Vector3& v) const
    {
        return Vector3(x - v.x, y - v.y, z - v.z);
    }

    Vector3 operator*(float s) const
    {
        return Vector3(x * s, y * s, z * s);
    }

    Vector3 operator*(Vector3 v) const
    {
        return Vector3(x * v.x, y * v.y, z * v.z);
    }

    Vector3 operator/(float d) const
    {
        return Vector3(x / d, y / d, z / d);
    }

    void Normalize(void)
    {
        float mag_sq = x * x + y * y;
        if (mag_sq > 0)
        {
            float one_over_mag = 1.0f / sqrtf(mag_sq);
            x *= one_over_mag;
            y *= one_over_mag;
            z *= one_over_mag;
        }
    }
#ifdef _DEBUG
    void Display(void)
    {
        Logger::GtLogInfo("Vector3 content:");
        Logger::GtLog("%f8.3 %8.3f %f8.3", x, y, z);
    }
#endif
};

inline Vector3 operator*(float s, const Vector3& v)
{
    return Vector3(s * v.x, s * v.y, s * v.z);
}
#pragma warning(default:4201)
//  启用匿名结构的警告
#pragma warning(pop)