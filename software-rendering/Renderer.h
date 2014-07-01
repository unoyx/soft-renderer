#pragma once
#include <string>
#include <vector>
#include <assert.h>
#include <Windows.h>
#include <d3d9.h>
#include "mathdef.h"
#include "matrix.h"
#include "util.h"
#include "Logger.h"
#include "Light.h"
#include "Primitive.h"

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


    // DDA 画线, 包括两端点 
    void DrawLine(Point p0, Point p1, uint32 c);

    void DrawPixel(int x, int y, uint32 c)
    {
        int idx = y * (pitch_ / 4) + x;
//        Logger::GtLogInfo("idx %d", idx);
        buffer_[idx] = c;
    }

    // Fixme GetDC的使用有限制条件，当前调用失败。参见 http://msdn.microsoft.com/en-us/library/windows/desktop/bb205894(v=vs.85).aspx
    void DrawText(Point pos, string text, uint32 c)
    {
        HDC hdc;
        HRESULT res = d3d_backbuffer_->GetDC(&hdc);
        if (FAILED(res))
        {
            Logger::GtLogError("d3d9 GetDC failed: %d", GetLastError());
            return;
        }

        RECT rect;
        rect.left = pos.x;
        rect.right = text.size() * 10 + pos.x;
        rect.top = pos.y;
        rect.bottom = pos.y + 20;

        SelectObject(hdc, font_);
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, c);
        ::DrawText(hdc, text.c_str(), text.length(), &rect, DT_LEFT);
        
        d3d_backbuffer_->ReleaseDC(hdc);
    }

    void BeginFrame(void);

    void EndFrame(void);

    // TODO 设置变换用的各个矩阵
    void SetMatrix(MatrixType t, const Matrix44 m);

    // TODO 设置光源
    void SetLight(Light *light);

    void SetLightingMode(LightingType t);

    // TODO 变换物体坐标至视图空间 *
    void ModelViewTransform(void);

    // TODO 背面剔除
    void BackfaceCulling(void);

    void SetBackfaceCulling(bool flag)
    {
        backface_culling_ = flag;
    }

    // TODO 计算光照
    void Lighting(void);

    // TODO 透视投影 *
    void Projection(void);

    // TODO 裁剪 *
    void Clipping(const Vector3 &w_min, const Vector3 &w_max);

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
private:
    void draw_line(Point p0, Point p1, uint32 c);
    void BresenhamLine(Point p0, Point p1, uint32 c);

    bool ClipLine3d(const Vector4 &beg, const Vector4 &end, 
                    const Vector3 &w_min, const Vector3 &w_max, Vector4 *res);

    void FlatTriangle(const Triangle *tri, uint32 color);
    void FlatTriangleUp(const Vector3 &v0, const Vector3 &v1, const Vector3 &v2, uint32 color);
    void FlatTriangleDown(const Vector3 &v0, const Vector3 &v1, const Vector3 &v2, uint32 color);
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
    
    Matrix44 model_view_;
    Matrix44 perspective_;
    Light *light_;
    RendPrimitive rend_primitive_;
    std::vector<Triangle> triangles_;

    bool flat_;
};

