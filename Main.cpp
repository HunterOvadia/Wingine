#include "Wingine.h"
#include "Core/Application.h"
#include "Core/Logger.h"
#include "Core/Memory.h"
#include "Core/Window.h"

void* operator new(size_t Size)
{
    return Memory::Allocate(Size);
}

void operator delete(void* Pointer)
{
    Memory::Free(Pointer);
}

int CALLBACK WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, PSTR CmdLine, int CmdShow)
{
    Application App;
    const WindowSettings WindowSettings =
    {
        .Title = "Wingine",
        .Position = Vector2<uint16>(100, 100),
        .Size = Vector2<uint16>(1280, 720),
    };

    if(App.Initialize(WindowSettings))
    {
        App.Start();
        App.Run();
    }

    App.Shutdown();
    Memory::Log();
    return 0;
}