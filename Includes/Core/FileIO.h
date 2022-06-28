#pragma once
#include "Defines.h"

struct ReadFileResult
{
    void* Content;
    uint32 ContentSize;
    bool IsValid() const
    {
        return Content && ContentSize > 0;
    }
};

class FileIO
{
public:
    static ReadFileResult ReadFile(const char* Filename);
};