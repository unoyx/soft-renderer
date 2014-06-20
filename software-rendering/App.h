#pragma once
// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER              // Allow use of features specific to Windows 7 or later.
#define WINVER 0x0700       // Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT        // Allow use of features specific to Windows 7 or later.
#define _WIN32_WINNT 0x0700 // Change this to the appropriate value to target other versions of Windows.
#endif

//#ifndef UNICODE
//#define UNICODE
//#endif

// C RunTime Header Files:
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <wchar.h>
#include <math.h>
// Windows Header Files:
#include <windows.h>
#include "Renderer.h"

#include "quaternion.h"

//
// DemoApp class declaration
//
class DemoApp
{
public:
    DemoApp();
    ~DemoApp();

    // Register the window class and call methods for instantiating drawing resources
    HWND Initialize(HINSTANCE inst, int width, int height);
    void Uninitialize(void);

    void Run(void);
    void Update(void);
private:
    // The windows procedure.
    static LRESULT CALLBACK WndProc(HWND hWnd,
                                    UINT message,
                                    WPARAM wParam,
                                    LPARAM lParam);
private:
    DemoApp(const DemoApp&);
    DemoApp& operator=(const DemoApp&);

    HWND hwind_;
    int width_;
    int height_;
    Renderer renderer_;
};