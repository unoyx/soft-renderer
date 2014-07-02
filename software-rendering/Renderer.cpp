#include "Renderer.h"
#include <assert.h>
#include "util.h"
#include "Camera.h"
#include "Light.h"

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
    ,light_(nullptr)
    ,flat_(false)
    ,light_type_(kNoLight)
    ,camera_(nullptr)
    ,color_(RGB(255, 0, 0))
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
    params.BackBufferCount = 1;
    params.MultiSampleType = D3DMULTISAMPLE_NONE;
    params.MultiSampleQuality = 0;
    params.SwapEffect = D3DSWAPEFFECT_DISCARD;
    params.hDeviceWindow = hwnd;
    params.Windowed = true;
    params.EnableAutoDepthStencil = false;
    params.AutoDepthStencilFormat = D3DFMT_D16;
    params.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
    // params.FullScreen_RefreshRateInHz =
    params.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

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
    assert(font_ != NULL);
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

void Renderer::SetCamera(Camera *camera)
{
    camera_ = camera;
}

// DDA 画线, 包括两端点 
void Renderer::DrawLine(Point p0, Point p1, uint32 c)
{
//    assert(0 <= p0.x && p0.x <= width_);
//    assert(0 <= p1.y && p1.y <= height_);
    // 线段在x,y方向的增量
    int dx = p1.x - p0.x;
    int dy = p1.y - p0.y;

    int steps = 0;
    if (abs(dy) > abs(dx))
    {
        steps = abs(dy);
    }
    else
    {
        steps = abs(dx);
    }

    // 整数计数器的增量
    float x_inc = abs(dx);
    float y_inc = abs(dy);
    // 整数计数器
    int x_m = 0;
    int y_m = 0;

    int x = p0.x;
    int y = p0.y;

    int x_step = (dx > 0) ? 1 : -1;
    int y_step = (dy > 0) ? 1 : -1;

    if (x >= 0 && x < width_ && y >= 0 && y < height_)
        DrawPixel(x, y, c);
    for (int k = 0; k < steps; ++k)
    {
        x_m += x_inc;
        if (x_m >= steps)
        {
            x_m -= steps;
            x += x_step;
        }
        y_m += y_inc;
        if (y_m >= steps)
        {
            y_m -= steps;
            y += y_step;
        }
        if (x >= 0 && x < width_ && y >= 0 && y < height_)
            DrawPixel(x, y, c);
    }
}

void Renderer::BresenhamLine(Point p0, Point p1, uint32 c)
{
    bool steep = abs(p1.y - p0.y) > abs(p1.x - p0.x);
    if (steep)
    {
        swap(p0.x, p0.y);
        swap(p1.x, p1.y);
    }
    if (p0.x > p1.x)
    {
        swap(p0.x, p1.x);
        swap(p0.y, p1.y);
    }
    int dx = p1.x - p0.x;
    int dy = abs(p1.y - p0.y);
    int err = dx / 2;
    int y_inc = (p0.y < p1.y) ? 1 : -1;
    int y = p0.y;
    for (int x = p0.x; x < p1.x; ++x)
    {
        if (steep)
        {
            DrawPixel(y, x, c);
        }
        else
        {
            DrawPixel(x, y, c);
        }
        err -= dy;
        if (err < 0)
        {
            y += y_inc;
            err += dx;
        }
    }

}

void Renderer::draw_line(Point p0, Point p1, uint32 c) {
	// 参数 c 为颜色值
	int dx = abs(p1.x - p0.x),
		dy = abs(p1.y - p0.y),
		yy = 0;

	if (dx < dy) {
		yy = 1;
		swap(p0.x, p0.y);
		swap(p1.x, p1.y);
		swap(dx, dy);
	}

	int ix = (p1.x - p0.x) > 0 ? 1 : -1,
		 iy = (p1.y - p0.y) > 0 ? 1 : -1,
		 cx = p0.x,
		 cy = p0.y,
		 n2dy = dy * 2,
		 n2dydx = (dy - dx) * 2,
		 d = dy * 2 - dx;

	if (yy) { // 如果直线与 x 轴的夹角大于 45 度
		while (cx != p1.x) {
			if (d < 0) {
				d += n2dy;
			} else {
				cy += iy;
				d += n2dydx;
			}
			DrawPixel(cy, cx, c);
			cx += ix;
		}
	} else { // 如果直线与 x 轴的夹角小于 45 度
		while (cx != p1.x) {
			if (d < 0) {
				d += n2dy;
			} else {
				cy += iy;
				d += n2dydx;
			}
			DrawPixel(cx, cy, c);
			cx += ix;
		}
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
    Flush();
    d3d_device_->Present(0, 0, 0, 0);
}

void Renderer::DrawPrimitive(Primitive *primitive)
{
    rend_primitive_ = RendPrimitive(primitive->size, primitive->material);

    for (int i = 0; i < rend_primitive_.size; ++i)
    {
        rend_primitive_.vertexs[i].position.SetVector3(primitive->position[i]);
        rend_primitive_.vertexs[i].position.w = 1.0f;
        rend_primitive_.vertexs[i].normal.SetVector3(primitive->normals[i]);
        rend_primitive_.vertexs[i].normal.w = 0.0f;
        rend_primitive_.vertexs[i].color = primitive->colors[i];
    }

    Matrix44 model_view = camera_->GetModelViewMatrix();
    ModelViewTransform(model_view);
    float z_far = camera_->get_far();
    float z_near = camera_->get_near();
    // Lighting();
    Matrix44 perspective = camera_->GetPerpectivMatrix();
    Projection(perspective);
    Clipping(z_near, z_far);
    Rasterization();
}

// rend_primitive 相机空间
void Renderer::ModelViewTransform(const Matrix44 &model_view)
{
    for (int i = 0; i < rend_primitive_.size; ++i)
    {
        Vector4 &position = rend_primitive_.vertexs[i].position;
        position = position * model_view;
        Vector4 &normal = rend_primitive_.vertexs[i].position;
        normal = normal * model_view;
    }
}

//void Renderer::Lighting(void)
//{
//    Material &mat = *rend_primitive_.material;
//
//    if (light_type_ == kNoLight)
//    {
//        return;
//    }
//    else if (light_type_ == kFlat)
//    {
//
//    }
//    else if (light_type_ == kGouraud)
//    {
//        for (int i = 0; i < rend_primitive_.size; ++i)
//        {
//            Vector3 L = rend_primitive_.position[i].GetVector3() - light_->position;
//            float dist = L.Magnitude();
//            L.Normalize();
//            Vector3 N = rend_primitive_.normals[i].GetVector3();
//            float cos_ang = L * N;
//            if (cos_ang < 0)
//            {
//                continue;
//            }
//            // 光强
//            float I = 1 / (light_->attenuation0 + light_->attenuation1 * dist + light_->attenuation2 * dist * dist);
//            // 环境光
//            Vector4 ambColor = light_->ambient * mat.ambient * I;
//            // 漫反射
//            float angle = N * L;
//            angle = max_t(angle, 0);
//            Vector4 difColor = light_->diffuse * mat.diffuse * angle * I;
//            Vector4 speColor = light_->specular * mat.specular * powf(angle, mat.specular.w) * I;
//            rend_primitive_.colors[i] = ambColor + difColor + speColor;
//        }
//    }
//}

// rend_primitive [N/R*x, N/T*y, F(z-N)/(F-N), z] / z
// 做完透视裁减，将图像变换至cvv
void Renderer::Projection(const Matrix44 &perspective)
{
    for (int i = 0; i < rend_primitive_.size; ++i)
    {
        Vector4 &position = rend_primitive_.vertexs[i].position;
        position = position * perspective;
    }
}

// 背面剔除，裁剪，透视除法
/*
void Renderer::Clipping(const Vector3 &w_min, const Vector3 &w_max)
{
//    assert((rend_primitive_.size % 3) == 0);
    // (rand_primitive_.size / 3) 向下截断

    static const int TRIANGLE_SIZE = 3;
    for (int i = 0; i < rend_primitive_.size; i += 3)
    {
        int vertex_in_cvv = 0;
        bool vertex_in_ccodes[TRIANGLE_SIZE] = {true, true, true};

        for (int j = 0; j < TRIANGLE_SIZE; ++j)
        {
            Vector4 &p0 = rend_primitive_.position[i + j];

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

        Vector4 *vtx = rend_primitive_.position + i;
        Vector4 *col = rend_primitive_.colors + i;
        
        Vector3 a = vtx[0].GetVector3();
        Vector3 b = vtx[1].GetVector3();
        Vector3 c = vtx[2].GetVector3();                                                        

        Vector3 n = CrossProduct(b - a, c - b);
        // FIXME 背面剔除，需确定判断是否正确
        if (backface_culling_ && n.z > 0)
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
            // and keep the sequence of triangle.
            bool ret = false;
            ret = ClipLine3d(vtx[v0], vtx[v1], w_min, w_max, &vtx[v1]);
            assert(ret);
            ret = ClipLine3d(vtx[v0], vtx[v2], w_min, w_max, &vtx[v2]);
            assert(ret);
            Triangle tri(vtx[v0], col[v0], vtx[v1], col[v1], vtx[v2], col[v2]);
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
            // and keep the sequence of triangle.
            Vector4 m;
            Vector4 n;
            bool ret = false;
            ret = ClipLine3d(vtx[v0], vtx[v1], w_min, w_max, &m);
            assert(ret);
            ret = ClipLine3d(vtx[v0], vtx[v2], w_min, w_max, &n);
            assert(ret);
            Triangle tri0(m, col[v0], vtx[v1], col[v1], vtx[v2], Vector4(0, 0, 0, 0));
            triangles_.push_back(tri0);
            Triangle tri1(n, col[v0], m, Vector4(0, 0, 0, 0), vtx[v2], col[v2]);
            triangles_.push_back(tri1);
        }
        else if (vertex_in_cvv == 3)
        {
            Triangle tri(vtx[v0], col[v0], vtx[v1], col[v1], vtx[v2], col[v2]);
            triangles_.push_back(tri);
        }
        else
        {
            assert(0);
        }
    }
}
*/


static void clip_near_plane(const RendVertex &a, const RendVertex &b, RendVertex *o)
{
    assert(a.position.z < 0);
    assert(b.position.z > 0);

    float k = (b.position.z - FLT_EPSILON) / (b.position.z - a.position.z);
    Vector4 pos = k * (a.position - b.position) + b.position;
    assert(pos.z > 0);
    Vector4 nor = k * (b.normal - a.normal) + a.normal;
    Vector4 col = k * (b.color - a.color) + a.color;

    o->position = pos;
    o->normal = nor;
    o->color = col;
}

static bool is_backface(const Vector3 &a, const Vector3 &b, const Vector3 &c)
{
    Vector3 n = CrossProduct(b - a, c - a);
    return (n.z > 0);
}

void Renderer::Clipping(float z_far, float z_near)
{
    static const int TRIANGLE_SIZE = 3;
    static const uint32 OVER_LEFT = 0x00000F;
    static const uint32 OVER_RIGHT = 0x0000F0;
    static const uint32 OVER_TOP = 0x000F00;
    static const uint32 OVER_BUTTON = 0x00F000;
    static const uint32 OVER_FAR = 0x0F0000;

    for (int i = 0; i < rend_primitive_.size; i += 3)
    {
        int vertex_before_xy = 0;
        uint32 vertex_culling_flag[TRIANGLE_SIZE] = {0};
        bool vertex_before_flag[TRIANGLE_SIZE] = {false, false, false};

        // 使用左，右，上，下，前相机平面进行剔除， 后相机平面进行裁剪
        for (int j = 0; j < TRIANGLE_SIZE; ++j)
        {
            Vector4 &p0 = rend_primitive_.vertexs[i + j].position;
            Logger::GtLog("Clip vertex:");
            p0.Display();

            if (p0.x < z_near)
                vertex_culling_flag[j] |= OVER_LEFT;
            if (p0.x > z_near)
                vertex_culling_flag[j] |= OVER_RIGHT;
            if (p0.y < z_near)
                vertex_culling_flag[j] |= OVER_BUTTON;
            if (p0.y > z_near)
                vertex_culling_flag[j] |= OVER_TOP;
            if (p0.z > z_far)
                vertex_culling_flag[j] |= OVER_FAR;

            if (p0.z > 0)
            {
                vertex_before_flag[j] = true;
                ++vertex_before_xy;
            }
        }

        uint32 culling = 0;
        for (int j = 0; j < TRIANGLE_SIZE; ++j)
        {
            culling &= vertex_culling_flag[j];
        }
        // 剔除
        if (culling)
        {
            continue;
        }

        RendVertex *vtx = rend_primitive_.vertexs + i;

        bool bf = is_backface(vtx[0].position.GetVector3(),
                              vtx[1].position.GetVector3(),
                              vtx[2].position.GetVector3());
        if (backface_culling_ && bf)
        {
            continue;
        }

        int v0 = 0;
        int v1 = 1;
        int v2 = 2;
        // 左手正向顺时针顺序排列顶点
        if (vertex_before_xy == 0)
        {
            continue;
        }
        else if (vertex_before_xy == 1)
        {
            if (vertex_before_flag[0])
            {

            }
            else if (vertex_before_flag[1])
            {
                v0 = 1; v1 = 2; v2 = 0;
            }
            else if (vertex_before_flag[2])
            {
                v0 = 2; v1 = 0; v2 = 1;
            }
            // make v0 is in. v1, v2 is out.
            // and keep the sequence of triangle.
            clip_near_plane(vtx[v1], vtx[v0], &vtx[v1]);
            clip_near_plane(vtx[v2], vtx[v0], &vtx[v2]);

            Triangle tri(vtx[v0], vtx[v1], vtx[v2]);
            triangles_.push_back(tri);
        }
        else if (vertex_before_xy == 2)
        {
            if (!vertex_before_flag[0])
            {
                //                v0 = 0; v1 = 1; v2 = 2;
            }
            else if (!vertex_before_flag[1])
            {
                v0 = 1; v1 = 2; v2 = 0;
            }
            else if (!vertex_before_flag[2])
            {
                v0 = 2; v1 = 0; v2 = 1;
            }
            // make v0 is out. v1, v2 is in. 
            // and keep the sequence of triangle.
            RendVertex m;
            RendVertex n;

            clip_near_plane(vtx[v0], vtx[v1], &m);
            clip_near_plane(vtx[v0], vtx[v2], &n);

            Triangle tri0(m, vtx[v1], vtx[v2]);
            triangles_.push_back(tri0);
            Triangle tri1(n, m, vtx[v2]);
            triangles_.push_back(tri1);
        }
        else if (vertex_before_xy == 3)
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

static void viewport_transform(int width, int height, Triangle *tri)
{
    assert(width > 0);
    assert(height > 0);
    int width_div2 = width / 2;
    int height_div2 = height / 2;

    for (int j = 0; j < 3; ++j)
    {
        assert(tri->v[j].position.z > 0);
        // 透视除法
        float div = 1 / tri->v[j].position.w;
        tri->v[j].position.x *= div;
        tri->v[j].position.y *= div;
        tri->v[j].position.z *= div;

        tri->v[j].position.Display();
//        tri->v[j].color.Display();

        // 将[x,y]变换至[width,height]的范围
        tri->v[j].position.x *= width_div2;
        tri->v[j].position.x += width_div2;
        tri->v[j].position.y *= -height_div2;
        tri->v[j].position.y += height_div2;
    }
}


// 三维空间的裁剪算法，精度不足
/*
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
        // 有计算误差
        res->x = clamp(res->x, w_min.x, w_max.x);
        res->y = clamp(res->y, w_min.y, w_max.y);
        return true;
    }
    assert(false);
    return false;
}
*/
#if 0
static float clip_line(Point p0, Point p1, int width, int height, int x, int y)
{

}

static void clip_viewport(int width, int height, Triangle *tri)
{
    static const int OVER_LEFT = 0x0000000F;
    static const int OVER_RIGHT = 0x000000F0;
    static const int OVER_TOP = 0x00000F00;
    static const int OVER_DOWN = 0x0000F000;
    static const int TRIANGLE_VERTEX = 3;

    int vertex_out = 0;
    int vertex_ccodes[TRIANGLE_VERTEX] = {0};

    for (int i = 0; i < TRIANGLE_VERTEX; ++i)
    {
        Vector4 &p = tri->v[i].position;
        if (p.x < 0)
            vertex_ccodes[i] |= OVER_LEFT;
        if (p.x >= width)
            vertex_ccodes[i] |= OVER_RIGHT;
        if (p.y < 0)
            vertex_ccodes[i] |= OVER_TOP;
        if (p.y >= height)
            vertex_ccodes[i] |= OVER_DOWN;

        if (vertex_ccodes[i])
            ++vertex_out;
    }

    if (vertex_out == 0)
    {
        return;
    }
    else if (vertex_out == 1)
    {

    }
    else if (vertex_out == 2)
    {

    }
    else if (vertex_out == 3)
    {

    }
}
#endif

// 直接光栅化，对象已在视口内
void Renderer::Rasterization()
{
    
    for (int i = 0; i < triangles_.size(); ++i)
    {
        viewport_transform(width_, height_, &triangles_[i]);

        if (flat_)
        {
//            uint32 c = vector4_to_ARGB32(triangles_[i].c[0]);

//            FlatTriangle(&triangles_[i], c);
        }
        else
        {
            Vector3 p0 = triangles_[i].v[0].position.GetVector3();
            uint32 c0 = vector4_to_ARGB32(triangles_[i].v[0].color);
            Vector3 p1 = triangles_[i].v[1].position.GetVector3();
            uint32 c1 = vector4_to_ARGB32(triangles_[i].v[1].color);
            Vector3 p2 = triangles_[i].v[2].position.GetVector3();
            uint32 c2 = vector4_to_ARGB32(triangles_[i].v[2].color);
            
            DrawLine(Point(p0.x, p0.y), Point(p1.x, p1.y), c0);
            DrawLine(Point(p1.x, p1.y), Point(p2.x, p2.y), c1);
            DrawLine(Point(p2.x, p2.y), Point(p0.x, p0.y), c2);
        }
    }
}


void Renderer::DiffTriangle(const Triangle *tri)
{
    RendVertex v0 = tri->v[0];
    Vector4 p0 = v0.position;
    RendVertex v1 = tri->v[1];
    Vector4 p1 = v1.position;
    RendVertex v2 = tri->v[2];
    Vector4 p2 = v2.position;

    if (equalf(p0.x, p1.x) && equalf(p1.x, p2.x))
        return;
    if (equalf(p0.y, p1.y) && equalf(p1.y, p2.y))
        return;

    // TODO 应该可以利用管线中三角形的性质进行优化d
    // 屏幕坐标，y轴朝下
    if (p0.y > p1.y)
    {
        swap(v0, v1);
    }
    if (p0.y > p2.y)
    {
        swap(v0, v2);
    }
    if (p1.y > p2.y)
    {
        swap(v1, v2);
    }
    
    // 平顶三角形
    /*       v0             v1
             _____________
             \           /
              \         /
               \       /
                \     /
                 \   /
                  \ /  
                  v2
    */
    if (equalf(p0.y, p1.y))
    {
        if (p0.x > p1.x)
            swap(v0, v1);
        FlatTriangleDown(v0, v1, v2, color);
    }
    // 平底三角形
    /*              v0
                    /\
                   /  \
                  /    \
                 /      \
                /        \
            v2 ------------ v1
    */
    else if (equalf(p1.y, p2.y))
    {
        if (p2.x > p1.x)
            swap(v1, v2);
        FlatTriangleUp(v0, v1, v2, color);
    }
    else
    {
        // 差值计算中点
        Vector3 m;
        m.y = p1.y;
        // m.x = (p0.x - p2.x ) / (p0.y - p2.y) * (m.y - p0.y) + p2.x;
        m.x = (p0.x * m.y - p0.x * p2.y - p2.x * m.y + p2.x * p0.y) / (v0.y - p2.y);
        // 朝左的三角形
        if (p1.x < m.x)
        {
            FlatTriangleUp(v0, m, v1, color);
            FlatTriangleDown(v1, m, v2, color);
        }
        // 朝右的三角形
        else
        {
            FlatTriangleUp(v0, v1, m, color);
            FlatTriangleDown(m, v1, v2, color);
        }
    }
}
    /*              v0
                    /\
                   /  \
                  /    \
                 /      \
                /        \
            v2 ------------ v1
    */
void Renderer::DiffTriangleUp(const RendVertex &v0, const RendVertex &v1, const RendVertex &v2)
{
    int dy = v1.position.y - v0.position.y;
    float dx_left = (v2.position.x - v0.position.x) / dy;
    float dx_right = (v1.position.x - v0.position.x) / dy;

    Vector4 dc_left = (v2.color - v0.color) / dy;
    Vector4 dc_right = (v1.color - v0.color) / dy;

    int x_begin = (int)v0.position.x;
    int x_end = (int)v0.position.x + 1;
    Vector4 color_begin = v0.color;
    Vector4 color_end = v0.color;

    int y = (int)v0.position.y;
    if (y < 0)
    {
        dy = -y;
        x_begin += dx_left * dy;
        x_end += dx_right * dy;
        color_begin += dc_left * dy;
        color_end += dc_right * dy;
    }
    if (x_begin < 0)
    {
        color_begin += dc_left * (-x_begin);
        x_begin = 0;
    }
    if (x_end >= width_)
    {
        color_end -= dc_right * (x_end - width_);
        x_end = width_ - 1;
    }

    for (int y = (int)v0.position.y; y < round(v1.position.y); ++y)
    {
        Vector4 dc = (color_begin - color_end) / (x_begin - x_end);
        Vector4 c = color_begin;
        int x = x_begin;
        for (int x = x_begin; x < x_end; ++x, c += dc)
        {
            DrawPixel(x, y, vector4_to_ARGB32(c));
        }
        x_begin += dx_left;
        x_end += dx_right;
        color_begin += dc_left;
        color_end += dc_right;
    }
}

    /*       v0             v1
             _____________
             \           /
              \         /
               \       /
                \     /
                 \   /
                  \ /  
                  v2
    */
void Renderer::DiffTriangleDown(const RendVertex &v0, const RendVertex &v1, const RendVertex &v2)
{

}

// 平底三角形，朝上
void Renderer::FlatTriangleUp(const Vector3 &v0, const Vector3 &v1, const Vector3 &v2, uint32 color)
{
    assert(equalf(v1.y, v2.y));
    assert(v0.y < v1.y);
    float dx_left = (v2.x - v0.x) / (v2.y - v0.y);
    float dx_right = (v1.x - v0.x) / (v2.y - v0.y);

    float sx = v0.x;
    float ex = v0.x;

    for (int i = round(v0.y); i < round(v1.y); ++i)
    {
        for (int j = round(sx); j < round(ex); ++j)
        {
            DrawPixel(j, i, color);
        }
        sx += dx_left;
        ex += dx_right;
    }
}

// 平顶三角形，朝下
void Renderer::FlatTriangleDown(const Vector3 &v0, const Vector3 &v1, const Vector3 &v2, uint32 color)
{
    assert(equalf(v0.y, v1.y));
    assert(v0.y < v2.y);
    float dx_left = (v2.x - v0.x) / (v2.y - v0.y);
    float dx_right = (v2.x - v1.x) / (v2.y - v0.y);

    float sx = v0.x;
    float ex = v1.x;

    for (int i = round(v0.y); i < round(v2.y); ++i)
    {
        for (int j = round(sx); j < round(ex); ++j)
        {
            DrawPixel(j, i, color);
        }
        sx += dx_left;
        ex += dx_right;
    }
}

/*
void Renderer::SetLight(Light *light)
{
    light_ = light;
}

void Renderer::Lighting(void)
{
    for (int i = 0; i < rend_primitive_.size; ++i)
    {
        Vector4 &vertex = rend_primitive_.position[i];

    }
}

void Renderer::SetLightingMode(LightingType t)
{
    light_type_ = t;
}
*/