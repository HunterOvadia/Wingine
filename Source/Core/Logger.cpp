#include "Core/Logger.h"
#include "Wingine.h"
#include <cstdio>

void Logger::Log(LogLevel Level, const char* Message, ...)
{
    char OutMessage[8192];
    
    char* ArgPtr;
    va_start(ArgPtr, Message);
    vsnprintf(OutMessage, 8192, Message, ArgPtr);
    va_end(ArgPtr);

    char OutMessage2[8192];
    sprintf_s(OutMessage2, "[%s]: %s\n", LevelStrings[static_cast<uint8>(Level)], OutMessage);
    
    WriteToConsole(OutMessage2, Level);
}

void Logger::WriteToConsole(const char* Message, LogLevel Level)
{
    HANDLE ConsoleHandle;
    if(Level >= LogLevel::Error)
    {
        ConsoleHandle = GetStdHandle(STD_ERROR_HANDLE);
    }
    else
    {
        ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    }

    SetConsoleTextAttribute(ConsoleHandle, LevelColors[static_cast<uint8>(Level)]);
    OutputDebugStringA(Message);

    const uint64 Length = strlen(Message);
    const LPDWORD NumberWritten = nullptr;
    WriteConsoleA(ConsoleHandle, Message, Length, NumberWritten, nullptr);
}

void Logger::WriteToFile(const char* Message)
{
}
