#pragma once
#include "Window.h"

struct FrameTimer
{
public:
    float64 Update();
    
private:
    void StartTimer();
    float64 GetTime() const;
    float64 UpdateFrameTime();

private:
    float64 CountsPerSecond = 0.0;
    int64 CounterStart = 0;
    int FrameCount = 0;
    int FPS = 0;
    int64 FrameTimeOld = 0;
    float64 FrameTime = 0;
};

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
    void Update(float64 DeltaTime);
    void Render();


    
private:
    bool IsRunning;
    Window* MainWindow;
    IRenderer* Renderer;

    FrameTimer Timer;
};
