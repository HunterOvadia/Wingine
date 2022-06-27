#pragma once
#include "Window.h"

class Application
{
public:
    Application();

public:
    bool Initialize(const WindowArgs& InArgs);
    void Shutdown();
    
    void Start();
    void Stop();
    void Run();

private:
    static Window* MakeWindow(const WindowArgs& InArgs);
    
    bool PumpMessages();
    void Update();
    void Render();
    
private:
    bool IsRunning;
    Window* MainWindow;
};
