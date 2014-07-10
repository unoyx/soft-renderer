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

enum MatrixType
{
    kModelView = 0,
    kPerspective = 1
};

enum ShadingMode
{
    kFrame = 0,
    kNoLightingEffect = 1,
    kFlat = 2,
    kGouraud = 3,
    kPhong = 4,
    kShadingModeCount = 5
};

class Texture2D;

class Renderer
{
public:
    Renderer(void);
    ~Renderer(void);
    void Initialize(HWND hwnd, int width, int height);
    void Uninitialize(void);

    Texture2D CreateTexture2D(void);
    void set_texture(Texture2D *texture)
    {
        if (texture_ && texture_->IsLocked())
        {
            texture_->UnLock();
        }
        texture_ = texture;
        if (!texture_->IsLocked())
        {
            texture_->Lock();
        }
    }

    Texture2D *get_texture(void)
    {
        return texture_;
    }

    void set_bumpmap(Texture2D *bumpmap)
    {
        if (bumpmap_ && bumpmap_->IsLoaded())
        {
            bumpmap_->UnLock();
        }
        bumpmap_ = bumpmap;
        if (!bumpmap_->IsLocked())
        {
            bumpmap_->Lock();
        }
    }

    Texture2D *get_bumpmap(void)
    {
        return bumpmap_;
    }

    void DrawLine(RendVertex v0, RendVertex v1);

    void set_pixel(int x, int y, uint32 c)
    {
        int idx = y * (pitch_ / 4) + x;
//        Logger::GtLogInfo("idx %d", idx);
        buffer_[idx] = c;
    }


    void set_text_color(uint32 color)
    {
        text_color_ = color;
    }

    void DrawScreenText(Point pos, string text)
    {
        text_pos_.push_back(pos);
        text_string_.push_back(text);
    }

    void DrawScreenText(int x, int y, string text)
    {
        text_pos_.push_back(Point(x, y));
        text_string_.push_back(text);
    }

    void BeginFrame(void);

    void EndFrame(void);

    void set_camera(Camera *camera)
    {
        camera_ = camera;
    }

    void set_light(Light *light)
    {
        light_ = light;
    }

    void set_backface_culling(bool flag)
    {
        backface_culling_ = flag;
    }

    void FlushText(void);

    // 绘制三角形
    void DrawPrimitive(Primitive *primitive);

    void switch_diff_perspective(void)
    {
        diff_perspective = !diff_perspective;
    }

    void switch_tri_up_down(void)
    {
        tri_up_down_ += 1;
        tri_up_down_ %= 3;
    }

    void set_shading_mode(ShadingMode mode)
    {
        shading_mode_ = mode;
    }

    void DisplayVertex(void);
    void DisplayTriangle(void);
    void DisplayStatus(void);
private:
    void set_one_over_z_buffer(int x, int y, float v)
    {
        one_over_z_buffer_[y * width_ + x] = v;
    }

    float get_one_over_z_buffer(int x, int y)
    {
        return one_over_z_buffer_[y * width_ + x];
    }
    // TODO 计算光照
    void Lighting(void);
    // TODO 变换物体坐标至视图空间 *
    void ModelViewTransform(const Matrix44 &model_view);
    // TODO 透视投影 *
    void Projection(const Matrix44 &perspective);

    // TODO 裁剪 *
    void Clipping(float z_far, float z_near);

    // TODO 光栅化 *
    void Rasterization(void);
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
    ShadingMode shading_mode_;

    int width_;
    int height_;
    int pitch_;
    // 指向backbufer内容
    uint32 *buffer_;
    // 1/z-buffer
    // 填充值分布在[1, 0)之间，数值越大，像素点越近
    float *one_over_z_buffer_;
    HFONT font_;
    
    Camera *camera_;
    Light *light_;
    Vector3 light_pos_;
    Material *mat_;

    RendPrimitive rend_primitive_;
    std::vector<Triangle> triangles_;

    std::vector<Point> text_pos_;
    std::vector<std::string> text_string_;
    uint32 text_color_;

    Texture2D *texture_;
    Texture2D *bumpmap_;
    bool flat_;
    bool diff_perspective;
    int tri_up_down_;
};

