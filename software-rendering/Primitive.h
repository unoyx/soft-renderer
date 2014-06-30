#pragma once
#include "mathdef.h"
#include "vector.h"

class Material;

class Primitive
{
public:
    Primitive(void)
        :size(size)
        ,position(nullptr)
        ,normals(nullptr)
        ,colors(nullptr)
        ,material(nullptr){}

    Primitive(int size, Material *material)
        :size(size)
        ,position(new Vector3[size])
        ,normals(new Vector3[size])
        ,colors(new Vector4[size])
        ,material(material) {}

    ~Primitive(void) 
    {
        Clear();
    }

    Primitive(Primitive &&r)
        :size(r.size)
        ,position(r.position)
        ,normals(r.normals)
        ,colors(r.colors)
        ,material(r.material)
    {
        r.size = 0;
        r.position = nullptr;
        r.normals = nullptr;
        r.colors = nullptr;
        r.material = nullptr;
    }

    Primitive &operator=(Primitive &&rhs)
    {
        Clear();
        size = rhs.size;
        position = rhs.position;
        normals = rhs.normals;
        colors = rhs.colors;
        material = rhs.material;

        rhs.size = 0;
        rhs.position = nullptr;
        rhs.normals = nullptr;
        rhs.colors = nullptr;
        rhs.material = nullptr;
    }

    void Clear(void)
    {
        if (position)
        {
            delete[] position;
            position = nullptr;
        }
        if (normals)
        {
            delete[] normals;
            normals = nullptr;
        }
        if (colors)
        {
            delete[] colors;
            colors = nullptr;
        }

        material = nullptr;
        size = 0;
    }
public:
    int size;
    Vector3 *position;
    Vector3 *normals;
    Vector4 *colors;
    Material *material;

//    Texture *texture;
//    Light *light;

private:
    Primitive(const Primitive&);
    Primitive& operator=(const Primitive&);
};

class RendPrimitive
{
public:
    RendPrimitive(void)
        :size(0)
        ,position(nullptr)
        ,normals(nullptr)
        ,colors(nullptr)
        ,material(nullptr) {}

    RendPrimitive(int size, Material *material)
        :size(size)
        ,position(new Vector4[size])
        ,normals(new Vector4[size])
        ,colors(new Vector4[size])
        ,material(material) {}

    ~RendPrimitive()
    {
        Clear();
    }

    RendPrimitive(RendPrimitive&& r)
        :size(r.size)
        ,position(r.position)
        ,normals(r.normals)
        ,colors(r.colors)
        ,material(material) 
    {
        r.size = 0;
        r.position = nullptr;
        r.normals = nullptr;
        r.colors = nullptr;
        r.material = nullptr;
    }

    RendPrimitive& operator=(RendPrimitive&& rhs)
    {
        Clear();
        size = rhs.size;
        position = rhs.position;
        normals = rhs.normals;
        colors = rhs.colors;
        material = rhs.material;

        rhs.size = 0;
        rhs.position = nullptr;
        rhs.normals = nullptr;
        rhs.colors = nullptr;
        rhs.material = nullptr;
        return *this;
    }

    void Clear(void)
    {
        if (position)
        {
            delete[] position;
            position = nullptr;
        }
        if (normals)
        {
            delete[] normals;
            normals = nullptr;
        }
        if (colors)
        {
            delete[] colors;
            colors = nullptr;
        }

        material = nullptr;
        size = 0;
    }
public:
    Vector4 *position;
    Vector4 *normals;
    Vector4 *colors;
    int size;
    Material *material;
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
    Vector3 n[3];
    Vector4 c[3];
//    Vector2 uv[3][2];
};

class Material
{
public:
    Vector4 diffuse;
    Vector4 ambient;
    Vector4 specular;
    Vector4 emissive;
    float power;
    Material(void)
        :power(0.0f){}
};