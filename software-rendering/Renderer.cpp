#include "Renderer.h"
#include <assert.h>
#include "Primitive.h"

Renderer::Renderer(void)
    :d3d9_(nullptr),
    d3d_device_(nullptr),
    d3d_backbuffer_(nullptr),
    width_(0),
    height_(0),
    pitch_(0),
    buffer_(nullptr)
{
}

Renderer::~Renderer(void)
{
}

void Renderer::Initialize(HWND hwnd, int width, int height)
{
    width_ = width;
    height_ = height;

    d3d9_ = Direct3DCreate9(D3D_SDK_VERSION);
    if (!d3d9_)
    {
        return;
    }
    D3DPRESENT_PARAMETERS params;
    memset(&params, 0, sizeof(params));

    params.BackBufferWidth = width;
    params.BackBufferHeight = height;
    params.BackBufferFormat = D3DFMT_A8R8G8B8;
    params.BackBufferCount = 0;
    params.MultiSampleType = D3DMULTISAMPLE_NONE;
    params.MultiSampleQuality = 0;
    params.SwapEffect = D3DSWAPEFFECT_DISCARD;
    params.hDeviceWindow = hwnd;
    params.Windowed = TRUE;
    params.EnableAutoDepthStencil = FALSE;
    // params.AutoDepthStencilFormat = 
    params.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
    // params.FullScreen_RefreshRateInHz =
    params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    HRESULT hr = d3d9_->CreateDevice(D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        hwnd,
        D3DCREATE_SOFTWARE_VERTEXPROCESSING,
        &params,
        &d3d_device_);
    if (FAILED(hr))
    {
        Logger::GtLogError("d3d9 CreateDevice failed: %d\n", GetLastError());
        SafeRelease(&d3d_device_);
        SafeRelease(&d3d9_);
        return;
    }

    hr = d3d_device_->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &d3d_backbuffer_);
    if (FAILED(hr))
    {
        Logger::GtLogError("d3d GetBackBuffer failed: %d\n", GetLastError());
        SafeRelease(&d3d_device_);
        SafeRelease(&d3d9_);
        return;
    }

    LOGFONT lfont;
    memset(&lfont, 0, sizeof(lfont));
    lfont.lfHeight = 12;
    lfont.lfWeight = 0;
    lfont.lfClipPrecision = CLIP_LH_ANGLES;
    lfont.lfQuality = NONANTIALIASED_QUALITY;
    strcpy_s(lfont.lfFaceName, "verdana");
    font_ = CreateFontIndirect(&lfont);
}

void Renderer::Uninitialize(void)
{
    DeleteObject(font_);
    SafeRelease(&d3d_backbuffer_);
    SafeRelease(&d3d_device_);
    SafeRelease(&d3d9_);
}

void Renderer::BeginFrame(void)
{
    d3d_device_->Clear(0, nullptr, D3DCLEAR_TARGET, 0, 0, 0);
    D3DLOCKED_RECT lockinfo;
    memset(&lockinfo, 0, sizeof(lockinfo));
    HRESULT res = d3d_backbuffer_->LockRect(&lockinfo, NULL, D3DLOCK_DISCARD);
    if (FAILED(res))
    {
        Logger::GtLogError("d3d LockRect failed: %d", GetLastError());
        return;
    }
    buffer_ = static_cast<uint32 *>(lockinfo.pBits);
    pitch_ = lockinfo.Pitch;
}

void Renderer::EndFrame(void)
{
    d3d_backbuffer_->UnlockRect();
    d3d_device_->Present(0, 0, 0, 0);
}

void Renderer::DrawPrimitive(Primitive *primitive)
{
    int size = primitive->size;

}

// DDA 画线, 包括两端点 
inline void Renderer::DrawLine(Point p0, Point p1, uint32 c)
{
    assert(0 <= p0.x && p0.x <= width_);
    assert(0 <= p1.y && p1.y <= height_);
    int dy = p1.y - p0.y;
    int dx = p1.x - p0.x;

    int steps = 0;
    if (abs(dy) > abs(dx))
    {
        steps = abs(dy);
    }
    else
    {
        steps = abs(dx);
    }

    float x_inc = static_cast<float>(dx) / static_cast<float>(steps);
    float y_inc = static_cast<float>(dy) / static_cast<float>(steps);

    float x = static_cast<float>(p0.x);
    float y = static_cast<float>(p0.y);

    DrawPixel(round(x), round(y), c);
    for (int k = 0; k < steps; ++k)
    {
        x += x_inc;
        y += y_inc;
        DrawPixel(round(x), round(y), c);
    }
}