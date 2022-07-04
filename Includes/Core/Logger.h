#pragma once
#include "Defines.h"

enum class LogLevel : uint8
{
    Trace,
    Debug,
    Info,
    Warn,
    Error,
    Fatal,
    cCount
};

static const char* LevelStrings[static_cast<uint8>(LogLevel::cCount)] = { "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL" };
static uint8 LevelColors[static_cast<uint8>(LogLevel::cCount)] = { 8, 1, 2, 6, 4, 64 };

class Logger
{
public:
    static void Log(LogLevel Level, const char* Message, ...);

private:
    static void WriteToConsole(const char* Message, LogLevel Level);
    static void WriteToFile(const char* Message);
};

#define WIN_LOG(level, message, ...) Logger::Log(LogLevel::level, message, ##__VA_ARGS__)
