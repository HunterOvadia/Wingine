#pragma once
#include "Window.h"

class IRenderer;
class Application
{
public:
    Application();

public:
    bool Initialize(const WindowSettings& InitialSettings);
    void Shutdown();
    
    void Start();
    void Stop();
    void Run();

private:
    Window* MakeWindow(const WindowSettings& InitialSettings);
    
    bool PumpMessages();
    void Update();
    void Render();
    
private:
    bool IsRunning;
    Window* MainWindow;
    IRenderer* Renderer;
};
