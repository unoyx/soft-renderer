#pragma once
#include <string>
#include <vector>
#include <assert.h>
#include <Windows.h>
#include <d3d9.h>
#include "mathdef.h"
#include "matrix.h"
#include "Primitive.h"

class Camera;
class Light;

using std::string;
using std::vector;

enum MatrixType
{
    kModelView = 0,
    kPerspective = 1
};

enum LightingType
{
    kNoLight = 0,
    kFlat = 1,
    kGouraud = 2
};

class Renderer
{
public:
    Renderer(void);
    ~Renderer(void);
    void Initialize(HWND hwnd, int width, int height);
    void Uninitialize(void);

    void DrawLine(RendVertex v0, RendVertex v1);

    void DrawPixel(int x, int y, uint32 c)
    {
        int idx = y * (pitch_ / 4) + x;
//        Logger::GtLogInfo("idx %d", idx);
        buffer_[idx] = c;
    }

    void Flush(void)
    {
        if (text_string_.empty())
            return;

        HDC hdc;
        HRESULT res = d3d_backbuffer_->GetDC(&hdc);
        if (FAILED(res))
        {
            Logger::GtLogError("d3d9 GetDC failed: %d", GetLastError());
            return;
        }
        SelectObject(hdc, font_);
        SetBkMode(hdc, TRANSPARENT);
        ::SetTextColor(hdc, color_);
        for (int i = 0; i < text_string_.size(); ++i)
        {
            RECT rect;
            rect.left = text_pos_[i].x;
            rect.right = text_string_[i].size() * 10 + rect.left;
            rect.top = text_pos_[i].y;
            rect.bottom = rect.top + 20;
            ::DrawText(hdc, text_string_[i].c_str(), text_string_[i].length(), &rect, DT_LEFT);
        }
        text_pos_.clear();
        text_string_.clear();
        d3d_backbuffer_->ReleaseDC(hdc);
    }
    
    void SetTextColor(uint32 color)
    {
        color_ = color;
    }

    // Fixme GetDC的使用有限制条件，当前调用失败。参见 http://msdn.microsoft.com/en-us/library/windows/desktop/bb205894(v=vs.85).aspx
    void DrawText(Point pos, string text)
    {
        text_pos_.push_back(pos);
        text_string_.push_back(text);
    }

    void BeginFrame(void);

    void EndFrame(void);

    // TODO 设置变换用的各个矩阵
    void SetMatrix(MatrixType t, const Matrix44 m);

    void SetCamera(Camera *camera);

    // TODO 设置光源
    void SetLight(Light *light);

    void SetLightingMode(LightingType t);

    // TODO 变换物体坐标至视图空间 *
    void ModelViewTransform(const Matrix44 &model_view);

    // TODO 背面剔除
    void BackfaceCulling(void);

    void SetBackfaceCulling(bool flag)
    {
        backface_culling_ = flag;
    }

    // TODO 计算光照
    void Lighting(void);

    // TODO 透视投影 *
    void Projection(const Matrix44 &perspective);

    // TODO 裁剪 *
    void Clipping(float z_far, float z_near);

    // TODO 视口变换 *
    void ViewportTransform(void);

    void ScreenMapping(void);

    // TODO 光栅化 *
    void Rasterization(void);

    // 绘制三角形
    void DrawPrimitive(Primitive *primitive);

    void SetFlat(bool flag)
    {
        flat_ = flag;
    }
    void draw_line(Point p0, Point p1, uint32 c);

private:
    void BresenhamLine(Point p0, Point p1, uint32 c);

    bool ClipLine3d(const Vector4 &beg, const Vector4 &end, 
                    const Vector3 &w_min, const Vector3 &w_max, Vector4 *res);
    void DiffTriangle(Triangle *tri);
    void DiffTriangleUp(const RendVertex &v0, const RendVertex &v1, const RendVertex &v2);
    void DiffTriangleDown(const RendVertex &v0, const RendVertex &v1, const RendVertex &v2);
private:
    Renderer(const Renderer&);
    Renderer& operator=(const Renderer&);

    IDirect3D9 *d3d9_;
    IDirect3DDevice9 *d3d_device_;
    IDirect3DSurface9 *d3d_backbuffer_;

    bool backface_culling_;
    LightingType light_type_;

    int width_;
    int height_;
    int pitch_;
    // 指向backbufer内容
    uint32 *buffer_;
    // z-buffer
    float *z_buffer_;
    HFONT font_;
    
    Camera *camera_;
    Light *light_;
    RendPrimitive rend_primitive_;
    std::vector<Triangle> triangles_;

    vector<Point> text_pos_;
    vector<string> text_string_;
    uint32 color_;

    bool flat_;
};

