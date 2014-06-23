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
#include "Primitive.h"

using std::string;
using std::vector;

class Renderer
{
public:
    Renderer(void);
    ~Renderer(void);
    void Initialize(HWND hwnd, int width, int height);
    void Uninitialize(void);
    
    void BeginFrame(void);
    void EndFrame(void);

    // 绘制三角形
    void DrawPrimitive(Primitive *primitive);

    // DDA 画线, 包括两端点 
    inline void DrawLine(Point p0, Point p1, uint32 c);

    void DrawPixel(int x, int y, uint32 c)
    {
        int idx = x * pitch_ / 4 + y;
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
private:
    Renderer(const Renderer&);
    Renderer& operator=(const Renderer&);

    IDirect3D9 *d3d9_;
    IDirect3DDevice9 *d3d_device_;
    IDirect3DSurface9 *d3d_backbuffer_;
    int width_;
    int height_;
    int pitch_;
    uint32 *buffer_;

    HFONT font_;
    
    Matrix44 view_model_;
    Matrix44 perspective_;
//    Light light_;
};

