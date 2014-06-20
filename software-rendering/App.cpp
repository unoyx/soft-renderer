#include "App.h"
#include "Logger.h"
#include "util.h"

// DemoApp constructor
DemoApp::DemoApp()
    :hwind_(nullptr),
    width_(0),
    height_(0)

{
}

// DemoApp destructor
// Releases the application's resources.
DemoApp::~DemoApp()
{
}

// Creates the application window and device-independent
// resources.
HWND DemoApp::Initialize(HINSTANCE inst, int width, int height)
{
    width_ = width;
    height_ = height;

    // Register the window class.
    WNDCLASSEX wcex    = {0};
    wcex.cbSize        = sizeof(wcex);
    wcex.style         = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc   = DemoApp::WndProc;
    wcex.cbClsExtra    = 0;
    wcex.cbWndExtra    = sizeof(LONG_PTR);
    wcex.hInstance     = inst;
    wcex.hbrBackground = (HBRUSH)(GetStockObject(WHITE_BRUSH));
    wcex.lpszMenuName  = nullptr;
    wcex.hCursor       = LoadCursor(0, IDC_ARROW);
    wcex.hIcon         = LoadIcon(0, IDI_APPLICATION);
    wcex.lpszClassName = "D2DDemoApp";

    (void)RegisterClassEx(&wcex);

    // Create the window.
    hwind_ = CreateWindow("D2DDemoApp",
                          "Direct2D Demo App",
                          WS_OVERLAPPED | WS_SYSMENU,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          width_,
                          height_,
                          nullptr,
                          nullptr,
                          inst,
                          this);
    if (!hwind_)
    {
        MessageBox(hwind_, "´´½¨´°¿ÚÊ§°Ü", "", MB_OK);
        return nullptr;
    }

    ShowWindow(hwind_, SW_SHOWNORMAL);
    UpdateWindow(hwind_);

    renderer_.Initialize(hwind_, width, height);

    return hwind_;
}

void DemoApp::Uninitialize(void)
{
    renderer_.Uninitialize();
}

void DemoApp::Run()
{
    for (;;)
    {
        MSG msg;
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            if (msg.message != WM_QUIT)
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            else
            {
                break;
            }
        }
        else
        {
            Update();
            while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }
}

void DemoApp::Update(void)
{
    renderer_.BeginFrame();

    for (int i = 0; i < 200; ++i)
    {
        renderer_.DrawLine(Point(0, i + 100), Point(100, i + 100), 0x00ff0000);
    }

//    renderer_.DrawText(Point2(100, 100), "hello world", 0x0000ff00);

    renderer_.EndFrame();
}

// Handles window messages.
LRESULT CALLBACK DemoApp::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_CREATE)
    {
        LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
        DemoApp *pDemoApp = (DemoApp *)pcs->lpCreateParams;

        ::SetWindowLongPtrW(hwnd,
                            GWLP_USERDATA,
                            PtrToUlong(pDemoApp));
    }
    else
    {
        DemoApp *pDemoApp = reinterpret_cast<DemoApp *>(static_cast<LONG_PTR>(::GetWindowLongPtrW(hwnd,
                                                                                                  GWLP_USERDATA)));
        if (pDemoApp)
        {
            switch (message)
            {
            case WM_DESTROY:
                PostQuitMessage(0);
                break;
            default:
                break;
            }
        }
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}

