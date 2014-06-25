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
    params.EnableAutoDepthStencil = false;
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

// DDA 画线, 包括两端点 
inline void Renderer::DrawLine(Point p0, Point p1, uint32 c)
{
//    assert(0 <= p0.x && p0.x <= width_);
//    assert(0 <= p1.y && p1.y <= height_);
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

    if (0 <= x && x <= width_ && 0 <= y && y <= height_)
        DrawPixel(round(x), round(y), c);
    for (int k = 0; k < steps; ++k)
    {
        x += x_inc;
        y += y_inc;
        if (0 <= x && x <= width_ && 0 <= y && y <= height_)
            DrawPixel(round(x), round(y), c);
    }
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

    rend_primitive_.Clear();
    triangles_.clear();
}

void Renderer::EndFrame(void)
{
    d3d_backbuffer_->UnlockRect();
    d3d_device_->Present(0, 0, 0, 0);
}

void Renderer::SetMatrix(MatrixType t, const Matrix44 m)
{
    switch (t)
    {
    case kModelView:
        model_view_ = m;
        break;
    case kPerspective:
        perspective_ = m;
        break;
    default:
        break;
    }
}

void Renderer::DrawPrimitive(Primitive *primitive)
{
    rend_primitive_ = RendPrimitive(primitive->size);

    for (int i = 0; i < rend_primitive_.size; ++i)
    {
        rend_primitive_.vertexes[i].SetVector3(primitive->vertexes[i]);
        rend_primitive_.vertexes[i].w = 1.0f;
    }

    ModelViewTransform();
    PerspectiveProjection();
    Clipping();
    ViewportTransform();
    Rasterization();
}

void Renderer::ModelViewTransform()
{
    for (int i = 0; i < rend_primitive_.size; ++i)
    {
        rend_primitive_.vertexes[i] = rend_primitive_.vertexes[i] * model_view_;
    }
}

//void Renderer::BackfaceCulling()
//{
//
//}

void Renderer::PerspectiveProjection()
{
    for (int i = 0; i < rend_primitive_.size; ++i)
    {
        rend_primitive_.vertexes[i] = rend_primitive_.vertexes[i] * perspective_;
    }
}

static bool in_cvv(const Vector4 &p, float w)
{
    if (p.x > w || p.x < -w)
        return false;
    if (p.y > w || p.y < -w)
        return false;
    if (p.z > w || p.z < 0)
        return false;
    return true;
}

void Renderer::Clipping()
{
    assert((rend_primitive_.size % 3) == 0);
    // (rand_primitive_.size / 3) 向下截断
    for (int i = 0; i < (rend_primitive_.size / 3); ++i)
    {
        Vector4 &p0 = rend_primitive_.vertexes[i];
        Vector4 &p1 = rend_primitive_.vertexes[i + 1];
        Vector4 &p2 = rend_primitive_.vertexes[i + 2];
        int vertex_in_cvv = 0;

        if (in_cvv(p0, p0.w))
            ++vertex_in_cvv;
        if (in_cvv(p1, p1.w))
            ++vertex_in_cvv;
        if (in_cvv(p2, p2.w))
            ++vertex_in_cvv;

        if (vertex_in_cvv == 0)
            continue;

        Triangle tri(p0, p1, p2);
        triangles_.push_back(tri);
    }
}

void Renderer::ViewportTransform()
{
    const int width_div2 = width_ / 2;
    const int height_div2 = height_ / 2;
    for (int i = 0; i < triangles_.size(); ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            // 透视除法
            triangles_[i].p[j] = (1 / triangles_[i].p[j].w) * triangles_[i].p[j];
            triangles_[i].p[j].x *= width_div2;
            triangles_[i].p[j].x += width_div2;
            triangles_[i].p[j].y *= -height_div2;
            triangles_[i].p[j].y += height_div2;
        }
    }
}

//static void ClipLine2d(Point &p0, Point &p1, int left, int right, int top, int bottom)
//{
//}

void Renderer::Rasterization()
{
    for (int i = 0; i < triangles_.size(); ++i)
    {
        Vector3 p0 = triangles_[i].p[0].GetVector3();
        Vector3 p1 = triangles_[i].p[1].GetVector3();
        Vector3 p2 = triangles_[i].p[2].GetVector3();

        DrawLine(Point(p0.x, p0.y), Point(p1.x, p1.y), 0x00ff0000);
        DrawLine(Point(p1.x, p1.y), Point(p2.x, p2.y), 0x00ff0000);
        DrawLine(Point(p2.x, p2.y), Point(p0.x, p0.y), 0x00ff0000);
    }
}