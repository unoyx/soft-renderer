#pragma once
#include <Windows.h>
#define DIRECTINPUT_VERSION 2048
#include <dinput.h>

#define INPUT_MGR_BUF_SIZE 256

enum MouseButton
{
    kMouseLeft = 0,
    kMouseRight = 1
};

class InputManager
{
public:
    InputManager(void);
    ~InputManager(void);

    bool Initialize(HINSTANCE inst, HWND wnd);

    void Update(void);
    int GetMouseMovingX(void);
    int GetMouseMovingY(void);
    int GetMouseX(void);
    int GetMouseY(void);
    bool MouseButtonDown(MouseButton button);
    bool KeyDown(DWORD key);
    bool KeyUp(DWORD key);
    bool KeyPressed(DWORD key);

private:
    InputManager(const InputManager&);
    InputManager &operator=(const InputManager&);

    IDirectInput8 *input_;
    IDirectInputDevice8 *mouse_dev_;
    IDirectInputDevice8 *keyboard_dev_;

    DIMOUSESTATE mouse_state_;
    UCHAR key_state_[INPUT_MGR_BUF_SIZE];
    UCHAR key_press_state_[INPUT_MGR_BUF_SIZE];
    int x_acc_;
    int y_acc_;
};

