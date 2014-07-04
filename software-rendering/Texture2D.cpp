#include "Texture2D.h"
#include <d3dx9tex.h>
#include <assert.h>
#include "mathdef.h"
#include "util.h"
#include "Logger.h"

Texture2D::Texture2D(IDirect3DDevice9 *device)
    :device_(device)
    ,texture_(nullptr)
    ,width_(0)
    ,height_(0)
    ,pitch_(0)
    ,data_(nullptr)
    ,format_(D3DFMT_UNKNOWN)
    ,is_loaded_(false)
    ,is_locked_(false)
{
}

Texture2D::~Texture2D(void)
{
    if (is_locked_)
        UnLock();
    if (is_loaded_)
        UnLoad();
}

bool Texture2D::Load(string filename)
{
    if (is_locked_ || is_loaded_)
    {
        assert(0);
        return false;
    }

    IDirect3DTexture9 *texture = nullptr;
    D3DXIMAGE_INFO info = {0};
    HRESULT hr = D3DXCreateTextureFromFileExA(device_, 
                                              filename.c_str(),
                                              D3DX_DEFAULT,
                                              D3DX_DEFAULT,
                                              D3DX_DEFAULT,
                                              D3DUSAGE_DYNAMIC,
                                              D3DFMT_FROM_FILE,
                                              D3DPOOL_DEFAULT,
                                              D3DX_FILTER_NONE,
                                              D3DX_FILTER_NONE,
                                              0,
                                              &info,
                                              nullptr,
                                              &texture);
    if (FAILED(hr))
    {
        Logger::GtLogError("load texture failed %s\n", filename.c_str());
        switch (hr)
        {
        case D3DERR_INVALIDCALL:
            break;
        case D3DERR_NOTAVAILABLE:
            break;
        case D3DERR_OUTOFVIDEOMEMORY:
            break;
        case D3DXERR_INVALIDDATA:
            break;
        case E_OUTOFMEMORY:
            break;
        default:
            break;
        }

        return false;
    }
    filename_ = filename;
    format_ = info.Format;
    texture_ = texture;
    width_ = info.Width;
    height_ = info.Height;

    is_loaded_ = true;
    return true;
}

void Texture2D::UnLoad(void)
{
    if (!is_loaded_ || is_locked_)
    {
        assert(0);
        return;
    }
    SafeRelease(&texture_);
    filename_.clear();
    format_ = D3DFMT_UNKNOWN;
    width_ = 0;
    height_ = 0;
    is_loaded_ = false;
}

bool Texture2D::Lock(void)
{
    if (!is_loaded_ || is_locked_)
    {
        assert(0);
        return false;
    }
    D3DLOCKED_RECT rect = {0};
    HRESULT hr = texture_->LockRect(0, &rect, nullptr, D3DLOCK_READONLY);
    if (FAILED(hr))
    {
        Logger::GtLogError("lock texture failed");
        return false;
    }
    pitch_ = rect.Pitch;
    data_ = static_cast<uint32 *>(rect.pBits);
    is_locked_ = true;
    return true;
}

void Texture2D::UnLock(void)
{
    if (!is_loaded_ || ! is_locked_)
    {
        assert(0);
        return;
    }
    HRESULT hr = texture_->UnlockRect(0);
    if (hr != D3D_OK)
    {
        Logger::GtLogError("unlock texture failed");
        return;
    }
    pitch_ = 0;
    data_ = nullptr;
    is_locked_ = false;
}

uint32 Texture2D::GetDate(int x, int y)
{
    if (!is_loaded_ || !is_locked_)
    {
        Logger::GtLogError("can't get data from texture without loaded or locked: %s", filename_.c_str());
        return 0;
    }
    if (x < 0 || x >= width_ || y < 0 || y >= height_)
    {
        assert(0);
        Logger::GtLogError("access texture is out of range");
    }
    
    uint32 c = data_[y * (pitch_ / 4) + x];
    if (format_ == D3DFMT_A8R8G8B8 || format_ == D3DFMT_X8R8G8B8)
    {
        uint32 a = ((format_ == D3DFMT_A8R8G8B8) ? ((c >> 24) & 0xFF) : 0);
        uint32 r = (c >> 16) & 0xFF;
        uint32 g = (c >> 8) & 0xFF;
        uint32 b = (c >> 0) & 0xFF;
        return ARGB32(0, r, g, b);
    } else 
    {
        return 0;
    }
}
