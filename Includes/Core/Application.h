#pragma once
#include "Window.h"

class DX11Renderer;
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
    void UpdateFrame();
    void RenderFrame();
    
private:
    bool IsRunning;
    Window* MainWindow;
    DX11Renderer* Renderer;
};

extern bool SetupScene();