#pragma once
#include "Window.h"

class Application
{
public:
    Application();
    ~Application();

public:
    bool Initialize(const WindowArgs& InArgs);
    void Shutdown();
    
    void Start();
    void Stop();
    void Run();

private:
    static Window* MakeWindow(const WindowArgs& InArgs);
    void PumpMessages();
    
private:
    bool IsRunning;
    Window* MainWindow;
};
