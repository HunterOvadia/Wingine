#include "Containers/String.h"

static uint32 CString_GetLength(const char* InString)
{
    uint32 Length = 0;
    while(InString[Length] != '\0')
    {
        ++Length;
    }
    return Length;
}

String::String()
    : Data(nullptr)
    , Size(0)
{
}

String::String(const char* Input)
{
    Data = Input;
    Size = CString_GetLength(Input);
}
