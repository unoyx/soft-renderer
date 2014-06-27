#include "Renderer.h"
#include <assert.h>
#include "Primitive.h"

Renderer::Renderer(void)
    :d3d9_(nullptr)
    ,d3d_device_(nullptr)
    ,d3d_backbuffer_(nullptr)
    ,width_(0)
    ,height_(0)
    ,pitch_(0)
    ,buffer_(nullptr)
    ,backface_culling_(false)
    ,z_buffer_(nullptr)
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

    z_buffer_ = new float[width_ * height_];

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
    if (z_buffer_)
    {
        delete[] z_buffer_;
        z_buffer_ = nullptr;
    }

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

//    if (0 <= x && x <= width_ && 0 <= y && y <= height_)
    DrawPixel(round(x), round(y), c);
    for (int k = 0; k < steps; ++k)
    {
        x += x_inc;
        y += y_inc;
//        if (0 <= x && x <= width_ && 0 <= y && y <= height_)
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
    memset(z_buffer_, 0, width_ * height_ * sizeof(float));
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
    ViewportTransform();
    Clipping(Vector3(0, 0, 0), Vector3(width_, height_, 1.0f));
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

void Renderer::ViewportTransform()
{
    const int width_div2 = width_ / 2;
    const int height_div2 = height_ / 2;

    for (int i = 0; i < rend_primitive_.size; ++i)
    {
        Vector4 &vertex = rend_primitive_.vertexes[i];
        float div = 1.0f / vertex.w;
        // 透视除法, 将对象变换至cvv
        vertex = vertex * div;
        // 将[x,y]变换至[width,height]的范围
        vertex.x *= width_div2;
        vertex.x += width_div2;
        vertex.y *= -height_div2;
        vertex.y += height_div2;
    }
}

bool Renderer::ClipLine3d(const Vector4 &beg, const Vector4 &end,
                          const Vector3 &w_min, const Vector3 &w_max, Vector4 *res)
{
    assert(res);

    static const int BUF_SIZE = 6;

    float x0 = beg.x;
    float y0 = beg.y;
    float z0 = beg.z; 
    float x_dt = end.x - beg.x;
    float y_dt = end.y - beg.y;
    float z_dt = end.z - beg.z;

    float p[BUF_SIZE] = {0};
    float q[BUF_SIZE] = {0};

    p[0] = -x_dt;
    q[0] = x0 - w_min.x;

    p[1] = x_dt;
    q[1] = w_max.x - x0;

    p[2] = -y_dt;
    q[2] = y0 - w_min.y;

    p[3] = y_dt;
    q[3] = w_max.y - y0;

    p[4] = -z_dt;
    q[4] = z0 - w_min.z; // 

    p[5] = z_dt;
    q[5] = w_max.z - z0;

    float u1 = 0.0f;
    float u2 = 1.0f;

    for (int i = 0; i < BUF_SIZE; ++i)
    {
        if (equalf(p[i], 0))
        {
            if (q[i] < 0)
                return false;
            continue;
        }
        float r = q[i] / p[i];
        // max of, from out to in.
        if (u1 < r && p[i] < 0)
        {
            u1 = r;
        }
        // min of, from in to out.
        if (u2 > r && p[i] > 0)
        {
            u2 = r;
        }
    }

    if (u1 > u2)
    {
        assert(false);
        return false;
    } 
    else
    {
        if (equalf(u1, 0))
        {
            *res = beg + (u2 * Vector4(x_dt, y_dt, z_dt, 0));
        }
        else if (equalf(u2, 1))
        {
            *res = beg + (u1 * Vector4(x_dt, y_dt, z_dt, 0));
        }
        res->x = static_cast<int>(res->x);
        res->y = static_cast<int>(res->y);
        res->z = static_cast<int>(res->z);
        assert(w_min.x <= res->x);
        assert(res->x <= w_max.x);
        assert(w_min.y <= res->y);
        assert(res->y <= w_max.y);
        assert(w_min.z <= res->z);
        assert(res->z <= w_max.z);
        return true;
    }
    assert(false);
    return false;
}

void Renderer::Clipping(const Vector3 &w_min, const Vector3 &w_max)
{
    assert((rend_primitive_.size % 3) == 0);
    // (rand_primitive_.size / 3) 向下截断

    static const int TRIANGLE_SIZE = 3;
    for (int i = 0; i < rend_primitive_.size; i += 3)
    {
        int vertex_in_cvv = 0;
        bool vertex_in_ccodes[TRIANGLE_SIZE] = {true, true, true};

        for (int j = 0; j < TRIANGLE_SIZE; ++j)
        {
            Vector4 &p0 = rend_primitive_.vertexes[i + j];
            if (p0.x < w_min.x || p0.x > w_max.x)
                vertex_in_ccodes[j] = false;
            if (p0.y < w_min.y || p0.y > w_max.y)
                vertex_in_ccodes[j] = false;
            if (p0.z < w_min.z || p0.z > w_max.z)
                vertex_in_ccodes[j] = false;
            if (vertex_in_ccodes[j])
            {
                ++vertex_in_cvv;
            }
        }

        Vector4 *vtx = rend_primitive_.vertexes + i;
        
        Vector3 a = vtx[0].GetVector3();
        Vector3 b = vtx[1].GetVector3();
        Vector3 c = vtx[2].GetVector3();                                                        

        Vector3 n = CrossProduct(b - a, c - b);
        // FIXME 背面剔除，需确定判断是否正确
        if (backface_culling_ && n.z < 0)
        {
            continue;
        }

        int v0 = 0;
        int v1 = 1;
        int v2 = 2;
        if (vertex_in_cvv == 0)
        {
            continue;
        }
        else if (vertex_in_cvv == 1)
        {
            if (vertex_in_ccodes[0])
            {

            }
            else if (vertex_in_ccodes[1])
            {
                v0 = 1; v1 = 2; v2 = 0;
            }
            else if (vertex_in_ccodes[2])
            {
                v0 = 2; v1 = 0; v2 = 1;
            }
            // make v0 is in. v1, v2 is out.
            bool ret = false;
            ret = ClipLine3d(vtx[v0], vtx[v1], w_min, w_max, &vtx[v1]);
            assert(ret);
            ret = ClipLine3d(vtx[v0], vtx[v2], w_min, w_max, &vtx[v2]);
            assert(ret);
            Triangle tri(vtx[v0], vtx[v1], vtx[v2]);
            triangles_.push_back(tri);
        }
        else if (vertex_in_cvv == 2)
        {
            if (!vertex_in_ccodes[0])
            {
//                v0 = 0; v1 = 1; v2 = 2;
            }
            else if (!vertex_in_ccodes[1])
            {
                v0 = 1; v1 = 2; v2 = 0;
            }
            else if (!vertex_in_ccodes[2])
            {
                v0 = 2; v1 = 0; v2 = 1;
            }
            // make v0 is out. v1, v2 is in. 
            Vector4 m;
            Vector4 n;
            bool ret = false;
            ret = ClipLine3d(vtx[v0], vtx[v1], w_min, w_max, &m);
            assert(ret);
            ret = ClipLine3d(vtx[v0], vtx[v2], w_min, w_max, &n);
            assert(ret);
            Triangle tri0(m, vtx[v1], vtx[v2]);
            triangles_.push_back(tri0);
            Triangle tri1(n, vtx[v2], n);
            triangles_.push_back(tri1);
        }
        else if (vertex_in_cvv == 3)
        {
            Triangle tri(vtx[v0], vtx[v1], vtx[v2]);
            triangles_.push_back(tri);
        }
        else
        {
            assert(0);
        }
    }
}

void Renderer::Rasterization()
{
    for (int i = 0; i < triangles_.size(); ++i)
    {
        Vector3 p0 = triangles_[i].p[0].GetVector3();
        Vector3 p1 = triangles_[i].p[1].GetVector3();
        Vector3 p2 = triangles_[i].p[2].GetVector3();

        DrawLine(Point(p0.x, p0.y), Point(p1.x, p1.y), 0x00ff0000);
        DrawLine(Point(p1.x, p1.y), Point(p2.x, p2.y), 0x0000ff00);
        DrawLine(Point(p2.x, p2.y), Point(p0.x, p0.y), 0x000000ff);
    }
}