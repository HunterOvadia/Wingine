#include "Wingine.h"
#include "Core/Application.h"
#include "Core/Logger.h"
#include "Core/Memory.h"
#include "Core/Window.h"

void* operator new(size_t Size)
{
    return GMemory.Allocate(Size);
}

void operator delete(void* Pointer)
{
    GMemory.Free(Pointer);
}

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
        App.Start();
        App.Run();
    }

    App.Shutdown();
    GMemory.Log();
    return 0;
}