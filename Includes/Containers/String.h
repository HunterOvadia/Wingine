#pragma once
#include "Defines.h"

class String
{
public:
    String();
    String(const char* Input);

public:
    const char* GetData() const { return Data; }

public:
    const char* operator*() const { return GetData(); }
    
private:
    const char* Data;
    uint32 Size;
};
