#pragma once
#include "mathdef.h"
#include "vector.h"
#include "Texture2D.h"

class Material;

class Primitive
{
public:
    Primitive(void)
        :size(size)
        ,positions(nullptr)
        ,normals(nullptr)
        ,colors(nullptr)
        ,uvs(nullptr)
        ,material(nullptr)
        ,texture(nullptr) {}

    Primitive(int size, Material *material, Texture2D *texture)
        :size(size)
        ,positions(new Vector3[size])
        ,normals(new Vector3[size])
        ,colors(new Vector4[size])
        ,uvs(new Vector2[size])
        ,material(material)
        ,texture(texture){}

    ~Primitive(void) 
    {
        Clear();
    }

    Primitive(Primitive &&r)
        :size(r.size)
        ,positions(r.positions)
        ,normals(r.normals)
        ,colors(r.colors)
        ,uvs(r.uvs)
        ,material(r.material)
        ,texture(r.texture)
    {
        r.size = 0;
        r.positions = nullptr;
        r.normals = nullptr;
        r.colors = nullptr;
        r.uvs = nullptr;
        r.material = nullptr;
        r.texture = nullptr;
    }

    Primitive &operator=(Primitive &&rhs)
    {
        Clear();
        size = rhs.size;
        positions = rhs.positions;
        normals = rhs.normals;
        colors = rhs.colors;
        uvs = rhs.uvs;
        material = rhs.material;
        texture = rhs.texture;

        rhs.size = 0;
        rhs.positions = nullptr;
        rhs.normals = nullptr;
        rhs.colors = nullptr;
        rhs.uvs = nullptr;
        rhs.material = nullptr;
        rhs.texture = nullptr;

        return *this;
    }

    void Clear(void)
    {
        if (positions)
        {
            delete[] positions;
            positions = nullptr;
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
        texture = nullptr;
        size = 0;
    }
public:
    int size;
    Vector3 *positions;
    Vector3 *normals;
    Vector4 *colors;
    Vector2 *uvs;
    Material *material;
    Texture2D *texture;

//    Light *light;
private:
};


class RendVertex
{
public:
    Vector4 position;
    Vector4 normal;
    Vector4 color;
    Vector2 uv;
};

class RendPrimitive
{
public:
    RendPrimitive(void)
        :size(0)
        ,vertexs(nullptr)
        ,material(nullptr) {}

    RendPrimitive(int size, Material *material)
        :size(size)
        ,vertexs(new RendVertex[size])
        ,material(material) {}

    ~RendPrimitive()
    {
        Clear();
    }

    RendPrimitive(RendPrimitive&& r)
        :size(r.size)
        ,vertexs(r.vertexs)
        ,material(material) 
    {
        r.size = 0;
        r.vertexs = nullptr;
        r.material = nullptr;
    }

    RendPrimitive& operator=(RendPrimitive&& rhs)
    {
        Clear();
        size = rhs.size;
        vertexs = rhs.vertexs;
        material = rhs.material;

        rhs.size = 0;
        rhs.vertexs = nullptr;
        rhs.material = nullptr;
        return *this;
    }

    void Clear(void)
    {
        if (vertexs)
        {
            delete[] vertexs;
            vertexs = nullptr;
        }
        material = nullptr;
        size = 0;
    }
public:
    int size;
    RendVertex *vertexs;
    Material *material;
private:
    RendPrimitive(const RendPrimitive&);
    RendPrimitive& operator=(const RendPrimitive&);
};


class Triangle
{
public:
    Triangle(void) {}
    Triangle(const RendVertex &v0,
             const RendVertex &v1,
             const RendVertex &v2)
    {
        v[0] = v0;
        v[1] = v1;
        v[2] = v2;
    }
    ~Triangle(void) {}

    RendVertex v[3];

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