#include "App.h"
// Provides the application entry point.

int WINAPI WinMain(HINSTANCE hInstance,
    HINSTANCE /* hPrevInstance */,
    LPSTR /* lpCmdLine */,
    int /* nCmdShow */)
{
    // Use HeapSetInformation to specify that the process should
    // terminate if the heap manager detects an error in any heap used
    // by the process.
    // The return value is ignored, because we want to continue running in the
    // unlikely event that HeapSetInformation fails.
    HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

    if (SUCCEEDED(CoInitialize(NULL)))
    {
        {
            App app;

            if (app.Initialize(hInstance, 800, 600))
            {
                app.Run();
            }
            app.Uninitialize();
        }
        CoUninitialize();
    }

    return 0;
}