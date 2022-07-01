#pragma once
#include "Core/Application.h"
#include "Core/Logger.h"
#include "Renderer/DX11Renderer.h"


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

        NewWindow->Shutdown();
        delete NewWindow;
        NewWindow = nullptr;
    }
    
    WIN_LOG(Fatal, "Application::MakeWindow - Failed to Initialize NewWindow!");
    return nullptr;
}

bool Application::Initialize(const WindowSettings& InitialSettings)
{
    MainWindow = MakeWindow(InitialSettings);
    if(!MainWindow)
    {
        return false;
    }
    
    Renderer = new DX11Renderer();
    if(!Renderer)
    {
        return false;
    }

    if(Renderer->Initialize(MainWindow))
    {
        MainWindow->Show();
        return true;
    }
    
    return false;
}

void Application::Shutdown()
{
    if(Renderer)
    {
        Renderer->Shutdown();
        delete Renderer;
        Renderer = nullptr;
    }
    
    if(MainWindow)
    {
        MainWindow->Shutdown();
        delete MainWindow;
        MainWindow = nullptr;
    }
    
    WIN_LOG(Info, "App Shutdown!");
}

void Application::Start()
{
    IsRunning = true;
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
            UpdateFrame();
            RenderFrame();
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

void Application::UpdateFrame()
{
    Renderer->UpdateScene();
}

void Application::RenderFrame()
{
    Renderer->RenderScene();
}
