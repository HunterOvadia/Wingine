#include "Wingine.h"
#include "Core/Application.h"
#include "Core/Logger.h"
#include "Core/Window.h"

int CALLBACK WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, PSTR CmdLine, int CmdShow)
{
    Application App;
    
    const WindowArgs Args =
    {
        .WindowTitle = "WindowTitle",
        .InitialPosition = Vector2<uint32>(100, 100),
        .InitialSize = Vector2<uint32>(1280, 720),
    };

    if(App.Initialize(Args))
    {
        App.Run();
    }

    App.Shutdown();
    return 0;
}