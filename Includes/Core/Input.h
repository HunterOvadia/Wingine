#pragma once
#include <dinput.h>

#include "Defines.h"
#include "Containers/Vector.h"

class Input
{
public:
    static void Initialize(HINSTANCE Instance, HWND Handle);
    static void PollInput();
    static bool IsKeyDown(uint8 KeyCode);
    static Vector2<int32> GetMousePosition();
private:
    static IDirectInputDevice8* Keyboard;
    static IDirectInputDevice8* Mouse;
    static LPDIRECTINPUT8 DirectInput;

    static DIMOUSESTATE MouseCurrentState;
    static DIMOUSESTATE MouseLastState;
    static uint8 KeyboardState[256];
};
