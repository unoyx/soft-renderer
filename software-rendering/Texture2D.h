#pragma once
#include <string>
#include <d3d9.h>
#include "typedef.h"

using std::string;

class Texture2D
{
public:
    Texture2D(IDirect3DDevice9 *device);
    bool Load(string filename);
    void Unload(void);
    bool Lock(void);
    void UnLock(void);

    uint32 GetDate(int x, int y);

    int get_width(void)
    {
        return width_;
    }

    int get_height(void)
    {
        return height_;
    }

    ~Texture2D(void);
private:
    Texture2D(void);

    IDirect3DDevice9 *device_;
    IDirect3DTexture9 *texture_;
    string filename_;
    int width_;
    int height_;
    bool is_loaded_;
    bool is_locked_;
};

