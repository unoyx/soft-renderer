#include "Texture2D.h"
#include <d3dx9tex.h>
#include <assert.h>
#include "mathdef.h"
#include "util.h"
#include "vector.h"
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
    ,filtering_(kNoneFiltering)
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
    return Load(filename, device_);
}

bool Texture2D::Load(string filename, IDirect3DDevice9 *device)
{
    if (is_locked_ || is_loaded_)
    {
        assert(0);
        return false;
    }

    IDirect3DTexture9 *texture = nullptr;
    D3DXIMAGE_INFO info = {0};
    HRESULT hr = D3DXCreateTextureFromFileExA(device, 
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

uint32 Texture2D::GetData(int x, int y)
{
    if (!is_loaded_ || !is_locked_)
    {
        Logger::GtLogError("can't get data from texture without loaded or locked: %s", filename_.c_str());
        return 0;
    }
    if (x < 0 || x >= width_ || y < 0 || y >= height_)
    {
        Logger::GtLogError("access texture is out of range");
        assert(0);
        return 0;
    }
    
    uint32 *d = static_cast<uint32*>(data_);
    uint32 c = d[y * pitch_ / 4 + x];
    if (format_ == D3DFMT_A8R8G8B8)
    {
        return c;
    }
    else if (format_ == D3DFMT_X8R8G8B8)
    {
        return (c | (0xFF << 24));
    } 

    else 
    {
        return 0;
    }
}

Vector4 Texture2D::GetDataUV(float u, float v)
{
    if (!is_loaded_ || !is_locked_)
    {
        Logger::GtLogError("can't get data from texture without loaded or locked: %s", filename_.c_str());
        return Vector4();
    }
    if (u < 0 || u > 1.0 || v < 0 || v > 1.0)
    {
        Logger::GtLogError("access texture is out of range");
        assert(0);
        return Vector4();
    }

    if (filtering_ == kNoneFiltering)
    {
        float x = u * (width_ - 1);
        float y = v * (height_ - 1);

        return GetDataVector4(static_cast<int>(x), static_cast<int>(y));
    }
    else if (filtering_ == kBilinterFiltering)
    {
        float x = u * (width_ - 2);
        float y = v * (height_ - 2);
        int x_ = static_cast<int>(x);
        int y_ = static_cast<int>(y);
        float x_off = x - x_;
        float y_off = y - y_;

        Vector4 d00 = GetDataVector4(x_, y_);
        Vector4 d01 = GetDataVector4(x_ + 1, y_);
        
        d00 = lerp(d00, d01, x_off);
        
        Vector4 d10 = GetDataVector4(x_, y_ + 1);
        Vector4 d11 = GetDataVector4(x_ + 1, y_ + 1);
        
        d10 = lerp(d10, d11, x_off);
        
        return lerp(d00, d10, y_off);
    }
        
    assert(0);
    return Vector4();
}

uint8 Texture2D::GetDumpData(int x, int y)
{
    if (!is_loaded_ || !is_locked_)
    {
        Logger::GtLogError("can't get data from texture without loaded or locked: %s", filename_.c_str());
        return 0;
    }
    if (x < 0 || x > width_ || y < 0 || y >= height_)
    {
        Logger::GtLogError("access texture is out of range");
        assert(0);
        return 0;
    }

    if (format_ == D3DFMT_L8)
    {
        uint8 *d = static_cast<uint8*>(data_);
        uint8 c = d[y * pitch_ + x];
        return c;
    }
    return 0;
}

Vector4 Texture2D::GetDataVector4(int x, int y)
{
    int idx = static_cast<int>(y) * pitch_ / 4 + static_cast<int>(x);
    uint32 *d = static_cast<uint32*>(data_);
    uint32 c = d[idx];
    if (format_ == D3DFMT_A8R8G8B8)
    {
        return ARGB32_to_vector4(c);
    }
    if (format_ == D3DFMT_X8R8G8B8)
    {
        return ARGB32_to_vector4(c | (0xFF << 24));
    }
    assert(0);
    return Vector4();
}
