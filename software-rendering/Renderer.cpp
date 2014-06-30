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
    ,light_(nullptr)
    ,flat_(false)
    ,light_type_(kNoLight)
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
    params.BackBufferFormat = D3DFMT_X8R8G8B8;
    params.BackBufferCount = 0;
    params.MultiSampleType = D3DMULTISAMPLE_NONE;
    params.MultiSampleQuality = 0;
    params.SwapEffect = D3DSWAPEFFECT_DISCARD;
    params.hDeviceWindow = hwnd;
    params.Windowed = TRUE;
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
    Logger::GtLogInfo("start x,y : %d,%d", p0.x, p0.y);
    Logger::GtLogInfo("end x,y : %d,%d", p1.x, p1.y);
    DrawPixel(x, y, c);
    Logger::GtLogInfo("x,y : %d,%d", x, y);
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
        Logger::GtLogInfo("x,y : %d,%d", x, y);
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
    //rend_primitive_ = RendPrimitive(primitive->size, primitive->material);

    //for (int i = 0; i < rend_primitive_.size; ++i)
    //{
    //    rend_primitive_.position[i].SetVector3(primitive->position[i]);
    //    rend_primitive_.position[i].w = 1.0f;
    //    rend_primitive_.normals[i].SetVector3(primitive->normals[i]);
    //    rend_primitive_.normals[i].w = 0.0f;
    //    rend_primitive_.colors[i] = primitive->colors[i];
    //}

    //ModelViewTransform();
    //// Lighting();
    //Projection();
    //Clipping(Vector3(-1, -1, 0), Vector3(1, 1, 1.0f));
    //Rasterization();
}

// rend_primitive 相机空间
void Renderer::ModelViewTransform(void)
{
    for (int i = 0; i < rend_primitive_.size; ++i)
    {
        Vector4 &position = rend_primitive_.position[i];
        position = position * model_view_;
        Vector4 &normal = rend_primitive_.normals[i];
        normal = normal * model_view_;
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
void Renderer::Projection(void)
{
    for (int i = 0; i < rend_primitive_.size; ++i)
    {
        Vector4 &position = rend_primitive_.position[i];
        position = position * perspective_;
        // FIXME 应该在进行裁剪以后做透视除法
        float div = 1 / abs(position.w);
        position *= div;
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
        // 有计算误差
        res->x = clamp(res->x, w_min.x, w_max.x);
        res->y = clamp(res->y, w_min.y, w_max.y);
        return true;
    }
    assert(false);
    return false;
}

// 背面剔除，裁剪，透视除法
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
            // and keep the sequence of triangle.
            Vector4 m;
            Vector4 n;
            bool ret = false;
            ret = ClipLine3d(vtx[v0], vtx[v1], w_min, w_max, &m);
            assert(ret);
            ret = ClipLine3d(vtx[v0], vtx[v2], w_min, w_max, &n);
            assert(ret);
            Triangle tri0(m, vtx[v1], vtx[v2]);
            triangles_.push_back(tri0);
            Triangle tri1(n, m, vtx[v2]);
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

// 直接光栅化，对象已在视口内
void Renderer::Rasterization()
{
    const int width_div2 = width_ / 2;
    const int height_div2 = height_ / 2;
    
    for (int i = 0; i < triangles_.size(); ++i)
    {
        const uint32 c[3] = {0x00ff0000, 0x0000ff00, 0x000000ff};
        static int k = 0;

        // 将[x,y]变换至[width,height]的范围
        for (int j = 0; j < 3; ++j)
        {
            triangles_[i].p[j].x *= width_div2;
            triangles_[i].p[j].x += width_div2;
            triangles_[i].p[j].y *= -height_div2;
            triangles_[i].p[j].y += height_div2;
        }

        if (flat_)
        {
            FlatTriangle(&triangles_[i], c[k++]);
            k %= 3;
        }
        else
        {
            Vector3 p0 = triangles_[i].p[0].GetVector3();
            Vector3 p1 = triangles_[i].p[1].GetVector3();
            Vector3 p2 = triangles_[i].p[2].GetVector3();
            
            uint32 red = ARGB32(255, 200, 0, 0);
            uint32 green = ARGB32(255, 0, 200, 0);
            uint32 blue = ARGB32(255, 0, 0, 200);
            draw_line(Point(p0.x, p0.y), Point(p1.x, p1.y), red);
            draw_line(Point(p1.x, p1.y), Point(p2.x, p2.y), green);
            draw_line(Point(p2.x, p2.y), Point(p0.x, p0.y), blue);
        }
    }
}

void Renderer::FlatTriangle(const Triangle *tri, uint32 color)
{
    Vector3 v0 = tri->p[0].GetVector3();
    Vector3 v1 = tri->p[1].GetVector3();
    Vector3 v2 = tri->p[2].GetVector3();

    if (equalf(v0.x, v1.x) && equalf(v1.x, v2.x))
        return;
    if (equalf(v0.y, v1.y) && equalf(v1.y, v2.y))
        return;

    // TODO 应该可以利用管线中三角形的性质进行优化d
    // 屏幕坐标，y轴朝下
    if (v0.y > v1.y)
    {
        swap(v0, v1);
    }
    if (v0.y > v2.y)
    {
        swap(v0, v2);
    }
    if (v1.y > v2.y)
    {
        swap(v1, v2);
    }
    
    // 平顶三角形
    if (equalf(v0.y, v1.y))
    {
        if (v0.x > v1.x)
            swap(v0, v1);
        FlatTriangleDown(v0, v1, v2, color);
    }
    // 平底三角形
    else if (equalf(v1.y, v2.y))
    {
        if (v2.x > v1.x)
            swap(v1, v2);
        FlatTriangleUp(v0, v1, v2, color);
    }
    else
    {
        // 差值计算中点
        Vector3 m;
        m.y = v1.y;
        // m.x = (v0.x - v2.x ) / (v0.y - v2.y) * (m.y - v0.y) + v2.x;
        m.x = (v0.x * m.y - v0.x * v2.y - v2.x * m.y + v2.x * v0.y) / (v0.y - v2.y);
        // 朝左的三角形
        if (v1.x < m.x)
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
