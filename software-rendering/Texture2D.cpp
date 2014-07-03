#include "Texture2D.h"
#include <d3dx9tex.h>
#include "util.h"
#include "Logger.h"

Texture2D::Texture2D(IDirect3DDevice9 *device)
    :device_(device)
    ,texture_(nullptr)
    ,width_(0)
    ,height_(0)
{
}


Texture2D::~Texture2D(void)
{
}

bool Texture2D::Load(string filename)
{
    IDirect3DTexture9 *texture = nullptr;
    D3DXIMAGE_INFO info = {0};
    HRESULT hr = D3DXCreateTextureFromFileEx(device_, 
                                             filename.c_str(),
                                             D3DX_DEFAULT,
                                             D3DX_DEFAULT,
                                             D3DX_DEFAULT,
                                             D3DUSAGE_DYNAMIC,
                                             D3DFMT_UNKNOWN,
                                             D3DPOOL_DEFAULT,
                                             D3DX_FILTER_NONE,
                                             D3DX_FILTER_NONE,
                                             0,
                                             &info,
                                             nullptr,
                                             &texture);
    if (hr != D3D_OK)
    {
        Logger::GtLogError("load texture failed %s\n", filename.c_str());
        return false;
    }
    width_ = info.Width;
    height_ = info.Height;


    is_loaded_ = true;
    return true;
}

void Texture2D::Unload(void)
{
    SafeRelease(&texture_);
    width_ = 0;
    height_ = 0;
    is_loaded_ = false;
}

bool Texture2D::Lock(void)
{
    is_locked_ = true;
}

void Texture2D::UnLock(void)
{
    is_locked_ = false;
}

uint32 Texture2D::GetDate(int x, int y)
{
    if (!is_loaded_ || !is_locked_)
    {
        Logger::GtLogError("can't access a texture without loaded or locked: %s", filename_.c_str());
    }

}
