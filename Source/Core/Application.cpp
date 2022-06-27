#pragma once
#include "Core/Application.h"
#include "Core/Logger.h"

Application::Application()
    : IsRunning(false)
    , MainWindow(nullptr)
{
}

Window* Application::MakeWindow(const WindowArgs& InArgs)
{
    Window* NewWindow = new Window(InArgs);
    if(NewWindow)
    {
        if(NewWindow->Initialize())
        {
            return NewWindow;
        }
        else
        {
            NewWindow->Shutdown();
            delete NewWindow;
            NewWindow = nullptr;
        }
    }
    
    WIN_LOG(Fatal, "Application::MakeWindow - Failed to Initialize NewWindow!");
    return nullptr;
}

bool Application::Initialize(const WindowArgs& InArgs)
{
    MainWindow = MakeWindow(InArgs);
    if(MainWindow)
    {
        MainWindow->Show();
        WIN_LOG(Info, "App Initialized!");
        return true;
    }

    return false;
}

void Application::Shutdown()
{
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
}

void Application::Render()
{
}
