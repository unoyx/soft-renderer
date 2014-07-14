#pragma once
#include <string>
#include <d3d9.h>
#include "Primitive.h"

class Renderer;

class Scene
{
public:
    Scene(void)
        :texture_(nullptr) {}

    ~Scene(void) {}

    void LoadFromXFile(std::string file, IDirect3DDevice9 *device);
    void Update(Renderer *renderer);

private:
    Scene(Scene &);
    Scene operator=(Scene &);

    Primitive primitive_;
    Material material_;
    Texture2D texture_;
};

