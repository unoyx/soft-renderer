#pragma once
#include <string>
#include <d3d9.h>
#include "typedef.h"

using std::string;

class Vector4;

enum FilteringType
{
    kNoneFiltering = 0,
    kBilinterFiltering = 1
};

class Texture2D
{
public:
    Texture2D(IDirect3DDevice9 *device);
    ~Texture2D(void);
    Texture2D(Texture2D &&rhs)
        :device_(rhs.device_)
        ,texture_(rhs.texture_)
        ,filename_(rhs.filename_)
        ,width_(rhs.width_)
        ,height_(rhs.height_)
        ,format_(rhs.format_)
        ,pitch_(rhs.pitch_)
        ,data_(rhs.data_)
        ,is_loaded_(rhs.is_loaded_)
        ,is_locked_(rhs.is_locked_)
        ,filtering_(rhs.filtering_)
    {
        rhs.device_ = nullptr;
        rhs.texture_ = nullptr;
        rhs.filename_.clear();
        rhs.width_ = 0;
        rhs.height_ = 0;
        rhs.format_ = D3DFMT_UNKNOWN;
        rhs.pitch_ = 0;
        rhs.data_ = nullptr;
        rhs.is_loaded_ = false;
        rhs.is_locked_ = false;
        rhs.filtering_ = kNoneFiltering;
    }

    Texture2D &operator=(Texture2D &&rhs)
    {
        if (is_locked_)
            UnLock();
        if (is_loaded_)
            UnLoad();
        device_ = rhs.device_;
        texture_ = rhs.texture_;
        filename_ = rhs.filename_;
        width_ = rhs.width_;
        height_ = rhs.height_;
        format_ = rhs.format_;
        pitch_ = rhs.pitch_;
        data_ = rhs.data_;
        is_loaded_ = rhs.is_loaded_;
        is_locked_ = rhs.is_locked_;
        filtering_ = rhs.filtering_;

        rhs.device_ = nullptr;
        rhs.texture_ = nullptr;
        rhs.filename_.clear();
        rhs.width_ = 0;
        rhs.height_ = 0;
        rhs.format_ = D3DFMT_UNKNOWN;
        rhs.pitch_ = 0;
        rhs.data_ = nullptr;
        rhs.is_loaded_ = false;
        rhs.is_locked_ = false;
        rhs.filtering_ = kNoneFiltering;

        return *this;
    }

    bool Load(string filename);
    bool Load(string filename, IDirect3DDevice9 *device);
    void UnLoad(void);
    bool Lock(void);
    void UnLock(void);

    uint32 GetData(int x, int y);
    Vector4 GetDataUV(float u, float v);
    uint8 GetDumpData(int x, int y);

    int get_width(void)
    {
        return width_;
    }

    int get_height(void)
    {
        return height_;
    }

    D3DFORMAT get_fromat(void)
    {
        return format_;
    }

    void set_filtering(FilteringType t)
    {
        filtering_ = t;
    }

    bool IsLoaded(void)
    {
        return is_loaded_;
    }

    bool IsLocked(void)
    {
        return is_locked_;
    }

private:
    Vector4 GetDataVector4(int x, int y);
private:
    Texture2D(void);

    IDirect3DDevice9 *device_;
    IDirect3DTexture9 *texture_;
    string filename_;
    int width_;
    int height_;
    D3DFORMAT format_;
    int pitch_;
    void *data_;
    bool is_loaded_;
    bool is_locked_;
    FilteringType filtering_;
};

