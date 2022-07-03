#include "Core/Application.h"
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
    
    Renderer = new DX11Renderer();
    if(Renderer->Initialize(MainWindow))
    {
        ShaderManager::Initialize(Renderer);
        TextureManager::Initialize(Renderer);
        
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
            Update();
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

void Application::Update()
{
    // TODO(HO): Scene/Render Commands
    Renderer->UpdateScene();
}

void Application::Render()
{
    Renderer->PreRender();

    // TODO(HO): Scene/Render Commands
    Renderer->RenderScene();

    Renderer->PostRender();
}
