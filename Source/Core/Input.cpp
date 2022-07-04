#include "Core/Input.h"

#include "Core/Logger.h"
#include "Renderer/DX11Renderer.h"

IDirectInputDevice8* Input::Keyboard = nullptr;
IDirectInputDevice8* Input::Mouse = nullptr;
LPDIRECTINPUT8 Input::DirectInput = nullptr;
DIMOUSESTATE Input::MouseCurrentState;
DIMOUSESTATE Input::MouseLastState;
uint8 Input::KeyboardState[256];

void Input::Initialize(HINSTANCE Instance, HWND Handle)
{
    HR_CHECK(DirectInput8Create(Instance, DIRECTINPUT_VERSION, IID_IDirectInput8, reinterpret_cast<void**>(&DirectInput), nullptr));
    HR_CHECK(DirectInput->CreateDevice(GUID_SysKeyboard, &Keyboard, nullptr));
    HR_CHECK(Keyboard->SetDataFormat(&c_dfDIKeyboard));
    HR_CHECK(Keyboard->SetCooperativeLevel(Handle, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));

    HR_CHECK(DirectInput->CreateDevice(GUID_SysMouse, &Mouse, nullptr));
    HR_CHECK(Mouse->SetDataFormat(&c_dfDIMouse));
    HR_CHECK(Mouse->SetCooperativeLevel(Handle, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND));
}

void Input::PollInput()
{
    Keyboard->Acquire();
    Mouse->Acquire();
    Mouse->GetDeviceState(sizeof(DIMOUSESTATE), &MouseCurrentState);
    Keyboard->GetDeviceState(sizeof(KeyboardState), &KeyboardState);

    if(MouseCurrentState.lX != MouseLastState.lX || MouseCurrentState.lY != MouseLastState.lY)
    {
        MouseLastState = MouseCurrentState;
    }
}

bool Input::IsKeyDown(uint8 KeyCode)
{
    return KeyboardState[KeyCode] & 0x80;
}

Vector2<int32> Input::GetMousePosition()
{
    return Vector2<int32>(MouseCurrentState.lX, MouseCurrentState.lY);
}
