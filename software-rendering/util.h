#pragma once

template<class Interface>
inline void SafeRelease(Interface **ppInterfaceToRelease)
{
    if (*ppInterfaceToRelease != NULL)
    {
        (*ppInterfaceToRelease)->Release();

        (*ppInterfaceToRelease) = NULL;
    }
}

#ifndef Assert
#if defined( DEBUG ) || defined( _DEBUG )
#define Assert(b) do {if (!(b)) {OutputDebugStringA("Assert: " #b "\n");}} while(0)
#else
#define Assert(b)
#endif //DEBUG || _DEBUG
#endif

enum PrimitiveType
{
    kTriangleOut = 0,
    kTriangleIn = 1,
    kSquare = 2,
    kPyramid = 3,
    kPrimitiveSize = 4
};

class Primitive;
void get_primitive(PrimitiveType t, Primitive *pri);
