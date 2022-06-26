#include "Wingine.h"
#include "Core/Window.h"

int CALLBACK WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, PSTR CmdLine, int CmdShow)
{
    const WindowArgs Args =
    {
        .WindowTitle = String("WindowTitle"),
        .InitialPosition = Vector2<uint32>(100, 100),
        .InitialSize = Vector2<uint32>(1280, 720),
    };

    Window AppWindow(Args);
    if (!AppWindow.Initialize())
    {
        // TODO(HO): Logging
        return 0;
    }
    
    AppWindow.Shutdown();
    return 0;
}