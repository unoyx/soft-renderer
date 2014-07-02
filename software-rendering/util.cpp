#include "util.h"
#include <utility>
#include "Primitive.h"

void get_primitive(PrimitiveType t, Primitive *pri)
{
    pri->Clear();

    if (t == kTriangleOut)
    {
        Primitive primitive(3, nullptr);
        primitive.colors[0] = Vector4(1.0, 0, 0, 1.0);
        primitive.colors[1] = Vector4(0, 1.0, 0, 1.0);
        primitive.colors[2] = Vector4(0, 0, 1.0, 1.0);

        primitive.position[0] = Vector3(0, 0.01, 1);
        primitive.position[1] = Vector3(0.25, 0, 0);
        primitive.position[2] = Vector3(-0.25, 0, 0);

        *pri = std::move(primitive);
    }
    else if (t == kTriangleIn)
    {
        Primitive primitive(3, nullptr);
        primitive.colors[0] = Vector4(1.0, 0, 0, 1.0);
        primitive.colors[1] = Vector4(0, 1.0, 0, 1.0);
        primitive.colors[2] = Vector4(0, 0, 1.0, 1.0);

        primitive.position[0] = Vector3(0, 0.01, -1);
        primitive.position[1] = Vector3(0.25, 0, 0);
        primitive.position[2] = Vector3(-0.25, 0, 0);

        *pri = std::move(primitive);
    }
    else if (t == kPyramid)
    {
        Primitive primitive(18, nullptr);
        for (int i = 0; i < 18; ++i)
        {
            primitive.colors[i] = Vector4(1.0f, 1.0, 1.0, 1.0);
        }
        primitive.position[0] = Vector3(0, 0, -1);
        primitive.position[1] = Vector3(0, 1, 0);
        primitive.position[2] = Vector3(1, 0, 0);

        primitive.position[3] = Vector3(1, 0, 0);
        primitive.position[4] = Vector3(0, 1, 0);
        primitive.position[5] = Vector3(0, 0, 1);

        primitive.position[6] = Vector3(0, 0, 1);
        primitive.position[7] = Vector3(0, 1, 0);
        primitive.position[8] = Vector3(-1, 0, 0);

        primitive.position[9] = Vector3(-1, 0, 0);
        primitive.position[10] = Vector3(0, 1, 0);
        primitive.position[11] = Vector3(0, 0, -1);

        primitive.position[12] = Vector3(0, 0, -1);
        primitive.position[13] = Vector3(1, 0, 0);
        primitive.position[14] = Vector3(0, 0, 1);

        primitive.position[15] = Vector3(0, 0, -1);
        primitive.position[16] = Vector3(0, 0, 1);
        primitive.position[17] = Vector3(-1, 0, 0);

        *pri = std::move(primitive);
    }
}