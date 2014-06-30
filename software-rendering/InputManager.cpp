#include "InputManager.h"
#include "util.h"

InputManager::InputManager(void)
    :input_(nullptr)
    ,mouse_dev_(nullptr)
    ,keyboard_dev_(nullptr)
    ,x_acc_(0)
    ,y_acc_(0)
{
    memset(&mouse_state_, 0, sizeof(mouse_state_));
    memset(key_state_, 0, sizeof(key_state_));
    memset(key_press_state_, 0, sizeof(key_press_state_));
}

InputManager::~InputManager(void)
{
    if (input_) 
    { 
        if (mouse_dev_) 
        { 
            mouse_dev_->Unacquire(); 
            SafeRelease(&mouse_dev_);
        }
        if (keyboard_dev_){
            keyboard_dev_->Unacquire(); 
            SafeRelease(&keyboard_dev_);
        }
        SafeRelease(&input_);
    } 
}

bool InputManager::Initialize(HINSTANCE inst, HWND wnd)
{
     HRESULT hr;

     // Create a direct input object
     hr = DirectInput8Create(inst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&input_, NULL); 

     if FAILED(hr){ 
         return false;
     }
     
     // Create a device for monitoring the mouse
     if (FAILED(input_->CreateDevice(GUID_SysMouse, &mouse_dev_, NULL)))
         return false;
     if (FAILED(mouse_dev_->SetDataFormat(&c_dfDIMouse)))
         return false;
     if (FAILED(mouse_dev_->SetCooperativeLevel(wnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE)))
         return false;
     if (FAILED(mouse_dev_->Acquire()))
         return false;

     // Create a device for monitoring the keyboard
     if (FAILED(input_->CreateDevice(GUID_SysKeyboard, &keyboard_dev_, NULL)))
         return false;
     if (FAILED(keyboard_dev_->SetDataFormat(&c_dfDIKeyboard)))
         return false;
     if (FAILED(keyboard_dev_->SetCooperativeLevel(wnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)))
         return false;
     if (FAILED(keyboard_dev_->Acquire()))
         return false;

     return true;
}

//
//   FUNCTION: getInput()
//
//   PURPOSE: Gets the state of the keyboard and mouse, if the device has been lost it trys to reaquire it
//
void InputManager::Update()
{
    HRESULT hr;

    hr = mouse_dev_->GetDeviceState(sizeof(mouse_state_),(LPVOID)&mouse_state_); 
    if (FAILED (hr))
    {
        // try and reacquire the input device
        mouse_dev_->Acquire();
    }

    keyboard_dev_->GetDeviceState(sizeof(key_state_), (LPVOID)key_state_);
    if (FAILED (hr))
    {
        // try and reacquire the input device
        keyboard_dev_->Acquire();
    }
    x_acc_ += mouse_state_.lX;
    y_acc_ += mouse_state_.lY;
}

//
//   FUNCTION: getMouseMovingX()
//
//   PURPOSE: returns the amount the mouse is moving in the X axis
//
int InputManager::GetMouseMovingX() 
{
    return mouse_state_.lX; 
}

//
//   FUNCTION: getMouseMovingY()
//
//   PURPOSE: returns the amount the mouse is moving in the Y axis
//
int InputManager::GetMouseMovingY() 
{
    return mouse_state_.lY; 
}

int InputManager::GetMouseX(void)
{
    return x_acc_;
}

int InputManager::GetMouseY(void)
{
    return y_acc_;
}

//   PURPOSE: Gets the state of the keyboard and mouse, if the device has been lost it trys to reaquire it
bool InputManager::MouseButtonDown(MouseButton button)
{
    int idx = static_cast<int>(button);
    if (mouse_state_.rgbButtons[idx] & 0x80){
        return true;
    } else {
        return false;
    }
}

//   PURPOSE: returns if a key is currently pressed.
bool InputManager::KeyDown(DWORD key)
{
    //check the state of the key
    if (key_state_[key] & 0x80){
        return true;
    } else {
        return false;
    }
}

//
//   FUNCTION: keyUp(DWORD key)
//
//   PURPOSE: returns if a key is currently not pressed.
//
bool InputManager::KeyUp(DWORD key)
{
    //check the state of the key
    if (key_state_[key] & 0x80){
        return false;
    } else {
        return true;
    }
}

//
//   FUNCTION: keyPress(DWORD key)
//
//   PURPOSE: returns if a key has been pressed and then depressed
//
bool InputManager::KeyPressed(DWORD key)
{
    //check for keydown
    if (KeyDown(key)){
        key_press_state_[key] = 1;
    }
    //check for key reaching the keydown state
    if (key_press_state_[key] == 1){
        //check for key release
        if (KeyUp(key))
            key_press_state_[key] = 2;
    }

    //check if key has been pressed and released
    if (key_press_state_[key] == 2){
        //reset the key status
        key_press_state_[key] = 0;
        return true;
    }
    return false;
}
