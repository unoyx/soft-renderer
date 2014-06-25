#pragma once
#include "mathdef.h"
#include "vector.h"

class Vector3;
class Vector4;

class Primitive
{
public:
    Primitive(void)
        :size(size)
        ,vertexes(nullptr) {}

    Primitive(int size)
        :size(size)
        ,vertexes(new Vector3[size]) {}

    ~Primitive(void) 
    {
        Clear();
    }

    Primitive(Primitive &&r)
        :size(r.size)
        ,vertexes(r.vertexes)
    {
        r.size = 0;
        r.vertexes = nullptr;
    }

    Primitive &operator=(Primitive &&rhs)
    {
        Clear();
        size = rhs.size;
        vertexes = rhs.vertexes;
        rhs.size = 0;
        rhs.vertexes = nullptr;
    }

    void Clear(void)
    {
        if (vertexes)
        {
            delete[] vertexes;
            vertexes = nullptr;
            size = 0;
        }
    }

    Vector3 *vertexes;
//    Vector3 *normals;
//    Vector4 *colors;
    int size;

//    Texture *texture;
//    Light *light;

private:
    Primitive(const Primitive&);
    Primitive& operator=(const Primitive&);
};

class RendPrimitive
{
public:
    RendPrimitive()
        :size(0)
        ,vertexes(nullptr) {}

    RendPrimitive(int size)
        :size(size)
        ,vertexes(new Vector4[size]) {}

    ~RendPrimitive()
    {
        Clear();
    }

    RendPrimitive(RendPrimitive&& r)
        :size(r.size)
        ,vertexes(r.vertexes)
    {
        r.size = 0;
        r.vertexes = nullptr;
    }

    RendPrimitive& operator=(RendPrimitive&& rhs)
    {
        Clear();
        size = rhs.size;
        vertexes = rhs.vertexes;
        rhs.size = 0;
        rhs.vertexes = nullptr;
        return *this;
    }

    void Clear(void)
    {
        if (vertexes)
        {
            delete[] vertexes;
            vertexes = nullptr;
            size = 0;
        }
    }

    Vector4 *vertexes;
//    Vector3 *normals;
    int size;
private:
    RendPrimitive(const RendPrimitive&);
    RendPrimitive& operator=(const RendPrimitive&);
};

class Triangle
{
public:
    Triangle(void) {}
    Triangle(const Vector4 &p0, const Vector4 &p1, const Vector4 &p2)
    {
        p[0] = p0;
        p[1] = p1;
        p[2] = p2;
    }
    ~Triangle(void) {}

    Vector4 p[3];
//    Vector3 n[3];
//    Vector2 uv[3][2];
};