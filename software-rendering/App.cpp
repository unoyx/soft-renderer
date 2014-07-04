#include "App.h"
#include "Logger.h"
#include "util.h"

// DemoApp constructor
App::App()
    :wnd_(nullptr)
    ,width_(0)
    ,height_(0)
    ,texture_(nullptr)
{
}

// DemoApp destructor
// Releases the application's resources.
App::~App()
{
}

// Creates the application window and device-independent
// resources.
HWND App::Initialize(HINSTANCE inst, int width, int height)
{
    width_ = width;
    height_ = height;

    // Register the window class.
    WNDCLASSEX wcex    = {0};
    wcex.cbSize        = sizeof(wcex);
    wcex.style         = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc   = App::WndProc;
    wcex.cbClsExtra    = 0;
    wcex.cbWndExtra    = sizeof(LONG_PTR);
    wcex.hInstance     = inst;
    wcex.hbrBackground = (HBRUSH)(GetStockObject(WHITE_BRUSH));
    wcex.lpszMenuName  = nullptr;
    wcex.hCursor       = LoadCursor(0, IDC_ARROW);
    wcex.hIcon         = LoadIcon(0, IDI_APPLICATION);
    wcex.lpszClassName = "SRApp";

    (void)RegisterClassEx(&wcex);

    // Create the window.
    wnd_ = CreateWindow("SRApp",
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
    if (!wnd_)
    {
        MessageBox(wnd_, "´´½¨´°¿ÚÊ§°Ü", "", MB_OK);
        return nullptr;
    }

    ShowWindow(wnd_, SW_SHOWNORMAL);
    UpdateWindow(wnd_);
    
    RECT client = {0};
    GetClientRect(wnd_, &client);

    input_mgr_.Initialize(inst, wnd_);
    renderer_.Initialize(wnd_, client.right - client.left, client.bottom - client.top);

    camera_.set_pos(Vector3(0, 0, -1));
    camera_.set_far(1.0);
    camera_.set_near(-1);
    camera_.set_fov(60);
    float aspect = static_cast<float>(width) / static_cast<float> (height);
    camera_.set_aspect(aspect);

    texture_ = renderer_.CreateTexture2D();
    bool ret = texture_.Load("D:\\src\\msvc\\software-rendering\\Debug\\tex2.jpg");
    assert(ret);
    ret = texture_.Lock();
    assert(ret);
    renderer_.SetTexture(&texture_);

    return wnd_;
}

void App::Uninitialize(void)
{
    renderer_.Uninitialize();
}

void App::Run()
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

void App::Update(void)
{
    static int elapsed_tick = GetTickCount();
    int current_tick = GetTickCount();
    int diff_tick = current_tick - elapsed_tick;
    elapsed_tick = current_tick;

    input_mgr_.Update();
    
    const float move_dist = 0.5f * diff_tick / 100.0f;
    if (input_mgr_.KeyDown(DIK_D))
        camera_.Move(Vector3(move_dist, 0, 0));
    if (input_mgr_.KeyDown(DIK_A))
        camera_.Move(Vector3(-move_dist, 0, 0));

    if (input_mgr_.KeyDown(DIK_W))
        camera_.Move(Vector3(0, 0, move_dist));
    if (input_mgr_.KeyDown(DIK_S))
        camera_.Move(Vector3(0, 0, -move_dist));

    if (input_mgr_.KeyDown(DIK_Q))
        camera_.Move(Vector3(0, move_dist, 0));
    if (input_mgr_.KeyDown(DIK_E))
        camera_.Move(Vector3(0, -move_dist, 0));

    const float rotate_angle = 3.0f * diff_tick / 100.0f;

    if (input_mgr_.KeyDown(DIK_UP))
        camera_.Rotate(rotate_angle, 0);
    if (input_mgr_.KeyDown(DIK_DOWN))
        camera_.Rotate(-rotate_angle, 0);
    if (input_mgr_.KeyDown(DIK_LEFT))
        camera_.Rotate(0, rotate_angle);
    if (input_mgr_.KeyDown(DIK_RIGHT))
        camera_.Rotate(0, -rotate_angle);

if (input_mgr_.KeyPressed(DIK_R))
{
    camera_.set_ori(Quat::GetIdentity());
    camera_.set_pos(Vector3(0, 0, -1));
}
    static bool bf_culling = true;

    if (input_mgr_.KeyPressed(DIK_B))
    {
        renderer_.SetBackfaceCulling(bf_culling);
        bf_culling = !bf_culling;
    }
    
    static bool bf_flat = true;
    if (input_mgr_.KeyPressed(DIK_F))
    {
        renderer_.SetFlat(bf_flat);
        bf_flat = !bf_flat;
    }

    renderer_.BeginFrame();

    static int p = 0;
    Primitive primitive;

    if (input_mgr_.KeyPressed(DIK_P))
    {
        p += 1;
        p %= kPrimitiveSize;

    } 
    renderer_.SetCamera(&camera_);

    //int w = texture_.get_width();
    //int h = texture_.get_height();
    //for (int x = 0; x < min_t(w, width_); ++x)
    //{
    //    for (int y = 0; y < min_t(h, height_); ++y)
    //    {
    //        uint32 c = texture_.GetDate(x, y);

    //        renderer_.DrawPixel(x, y, c);
    //    }
    //}


    get_primitive((PrimitiveType)p, &primitive);
    renderer_.DrawPrimitive(&primitive);

    //for (int i = 0; i < width_; ++i)
    //{
    //    for (int j = 0; j < height_; ++j)
    //    {
    //        renderer_.DrawPixel(j, i, 0x00ff0000);
    //    }
    //}

//    renderer_.DrawLine(Point(400, 0), Point(700, 300), 0x00ff0000);

//    renderer_.DrawText(Point2(100, 100), "hello world", 0x0000ff00);

    renderer_.EndFrame();
}

// Handles window messages.
LRESULT CALLBACK App::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_CREATE)
    {
        LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
        App *pDemoApp = (App *)pcs->lpCreateParams;

        ::SetWindowLongPtrW(hwnd,
                            GWLP_USERDATA,
                            PtrToUlong(pDemoApp));
    }
    else
    {
        App *pDemoApp = reinterpret_cast<App *>(static_cast<LONG_PTR>(::GetWindowLongPtrW(hwnd, GWLP_USERDATA)));
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

