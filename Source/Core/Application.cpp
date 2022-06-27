#pragma once
#include "Core/Application.h"
#include "Core/Logger.h"

Application::Application()
    : IsRunning(false)
    , MainWindow(nullptr)
{
}

Application::~Application()
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

        WIN_LOG(Fatal, "Failed to Initialize Window!");
    }

    if(NewWindow)
    {
        delete NewWindow;
        NewWindow = nullptr;
    }
    
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
    Start();
    while(IsRunning)
    {
        PumpMessages();
        // Update
        // Render
    }
}

void Application::PumpMessages()
{
    MSG Message;
    while(PeekMessageA(&Message, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&Message);
        DispatchMessage(&Message);
        
        if(Message.message == WM_QUIT)
        {
            IsRunning = false;
            break;
        }
    }
}
