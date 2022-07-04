#include "Core/Application.h"

#include "Core/Input.h"
#include "Core/Logger.h"
#include "Core/ShaderManager.h"
#include "Renderer/DX11Renderer.h"
#include "Core/TextureManager.h"

Application::Application()
    : IsRunning(false)
    , MainWindow(nullptr)
    , Renderer(nullptr)
{
}

Window* Application::MakeWindow(const WindowSettings& InitialSettings)
{
    Window* NewWindow = new Window();
    if(NewWindow)
    {
        if(NewWindow->Initialize(InitialSettings))
        {
            return NewWindow;
        }
    }

    if(NewWindow)
    {
        NewWindow->Shutdown();
        SAFE_DELETE(NewWindow);
    }
    
    WIN_LOG(Fatal, "Application::MakeWindow - Failed to Initialize NewWindow!");
    return nullptr;
}

bool Application::Initialize(const WindowSettings& InitialSettings)
{
    MainWindow = MakeWindow(InitialSettings);
    Input::Initialize(MainWindow->GetInstance(), MainWindow->GetHandle());
    
    Renderer = new DX11Renderer();
    ShaderManager::Initialize(Renderer);
    TextureManager::Initialize(Renderer);
    
    if(Renderer->Initialize(MainWindow))
    {

        
        MainWindow->Show();
        return true;
    }
    
    return false;
}

void Application::Shutdown()
{
    TextureManager::Shutdown();
    ShaderManager::Shutdown();
    
    if(Renderer)
    {
        Renderer->Shutdown();
        SAFE_DELETE(Renderer);
    }
    
    if(MainWindow)
    {
        MainWindow->Shutdown();
        SAFE_DELETE(MainWindow);
    }
    
    WIN_LOG(Info, "App Shutdown!");
}

void Application::Start()
{
    IsRunning = true;
    Renderer->InitializeScene();
}

void Application::Stop()
{
    IsRunning = false;
}

void Application::Run()
{
    while(IsRunning)
    {
        if(PumpMessages())
        {
            Input::PollInput();
            Update(Timer.Update());
            Render();
        }
    }
}

bool Application::PumpMessages()
{
    MSG Message;
    while(PeekMessageA(&Message, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&Message);
        DispatchMessage(&Message);
        if(Message.message == WM_QUIT)
        {
            Stop();
            break;
        }
    }

    return IsRunning;
}

void Application::Update(float64 DeltaTime)
{
    // TODO(HO): Scene/Render Commands
    Renderer->UpdateScene(DeltaTime);
}

void Application::Render()
{
    Renderer->PreRender();

    // TODO(HO): Scene/Render Commands
    Renderer->RenderScene();

    Renderer->PostRender();
}

float64 FrameTimer::Update()
{
    FrameCount++;

    if(GetTime() > 1.0f)
    {
        FPS = FrameCount;
        FrameCount = 0;
        StartTimer();
    }

    FrameTime = UpdateFrameTime();
    //WIN_LOG(Info, "FPS: %i | FrameTime: %f", FPS, FrameTime);
    return FrameTime;
}

void FrameTimer::StartTimer()
{
    LARGE_INTEGER FrequencyCount;
    QueryPerformanceFrequency(&FrequencyCount);

    CountsPerSecond = static_cast<float64>(FrequencyCount.QuadPart);

    QueryPerformanceCounter(&FrequencyCount);
    CounterStart = FrequencyCount.QuadPart;
}

float64 FrameTimer::GetTime() const
{
    LARGE_INTEGER CurrentTime;
    QueryPerformanceCounter(&CurrentTime);
    return static_cast<float64>(CurrentTime.QuadPart - CounterStart) / CountsPerSecond;
}

float64 FrameTimer::UpdateFrameTime()
{
    LARGE_INTEGER CurrentTime;
    QueryPerformanceCounter(&CurrentTime);

    int64 TickCount = CurrentTime.QuadPart - FrameTimeOld;
    FrameTimeOld = CurrentTime.QuadPart;

    if(TickCount < 0)
    {
        TickCount = 0;
    }

    return static_cast<float32>(TickCount)/CountsPerSecond;
}
