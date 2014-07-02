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
    kPyramid = 2,
    kPrimitiveSize = 3
};

class Primitive;
void get_primitive(PrimitiveType t, Primitive *pri);
