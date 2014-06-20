#pragma once
#include "mathdef.h"

class Vector3;

class Primitive
{
public:
    // TODO ���Ƿ��䡢���ƺ�����
    Primitive(void) :vertexes(nullptr), normals(nullptr), colors(nullptr) {}
    ~Primitive(void) {}

    Vector3 *vertexes;
    Vector3 *normals;
    uint32 *colors;
    int32 size;
private:
    Primitive(const Primitive&);
    Primitive& operator=(const Primitive&);
};

