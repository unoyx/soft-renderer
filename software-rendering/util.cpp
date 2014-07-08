#include "util.h"
#include <utility>
#include "Primitive.h"

void get_primitive(PrimitiveType t, Primitive *pri)
{
    pri->Clear();

    if (t == kTriangleOut)
    {
        Primitive primitive(3, nullptr, nullptr);
        //primitive.colors[0] = Vector4(1.0, 0, 0, 1.0);
        //primitive.colors[1] = Vector4(0, 1.0, 0, 1.0);
        //primitive.colors[2] = Vector4(0, 0, 1.0, 1.0);

        primitive.colors[0] = Vector4(1.0, 1.0, 1.0, 1.0);
        primitive.colors[1] = Vector4(1.0, 1.0, 1.0, 1.0);
        primitive.colors[2] = Vector4(1.0, 1.0, 1.0, 1.0);

        primitive.positions[0] = Vector3(0, 0, 1);
        primitive.positions[1] = Vector3(0.25, 0, 0);
        primitive.positions[2] = Vector3(-0.25, 0, 0);

        primitive.uvs[0] = Vector2(0, 0);
        primitive.uvs[1] = Vector2(0, 1);
        primitive.uvs[2] = Vector2(1, 0);

        *pri = std::move(primitive);
    }
    else if (t == kTriangleIn)
    {
        Primitive primitive(3, nullptr, nullptr);
        primitive.colors[0] = Vector4(1, 1, 1, 1);
        primitive.colors[1] = Vector4(1, 1, 1, 1);
        primitive.colors[2] = Vector4(1, 1, 1, 1);

        primitive.positions[0] = Vector3(0, 0.01f, -1);
        primitive.positions[1] = Vector3(0.25, 0, 0);
        primitive.positions[2] = Vector3(-0.25, 0, 0);

        primitive.uvs[0] = Vector2(0, 0);
        primitive.uvs[1] = Vector2(0, 1);
        primitive.uvs[2] = Vector2(1, 0);

        *pri = std::move(primitive);
    }
    else if (t == kPyramid)
    {
        Primitive primitive(18, nullptr, nullptr);
        for (int i = 0; i < 18; ++i)
        {
            primitive.colors[i] = Vector4(1.0f, 1.0, 1.0, 1.0);
        }
        primitive.positions[0] = Vector3(0, 0, -1);
        primitive.positions[1] = Vector3(0, 1, 0);
        primitive.positions[2] = Vector3(1, 0, 0);

        primitive.positions[3] = Vector3(1, 0, 0);
        primitive.positions[4] = Vector3(0, 1, 0);
        primitive.positions[5] = Vector3(0, 0, 1);

        primitive.positions[6] = Vector3(0, 0, 1);
        primitive.positions[7] = Vector3(0, 1, 0);
        primitive.positions[8] = Vector3(-1, 0, 0);

        primitive.positions[9] = Vector3(-1, 0, 0);
        primitive.positions[10] = Vector3(0, 1, 0);
        primitive.positions[11] = Vector3(0, 0, -1);

        primitive.positions[12] = Vector3(0, 0, -1);
        primitive.positions[13] = Vector3(1, 0, 0);
        primitive.positions[14] = Vector3(0, 0, 1);

        primitive.positions[15] = Vector3(0, 0, -1);
        primitive.positions[16] = Vector3(0, 0, 1);
        primitive.positions[17] = Vector3(-1, 0, 0);

        *pri = std::move(primitive);
    }
}