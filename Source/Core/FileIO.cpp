#include "Core/FileIO.h"

#include "Core/Application.h"
#include "Core/Logger.h"

ReadFileResult FileIO::ReadFile(const char* Filename)
{
    ReadFileResult Result = {};
    
    HANDLE FileHandle = CreateFileA(Filename, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if(FileHandle != INVALID_HANDLE_VALUE)
    {
        LARGE_INTEGER FileSize;
        if(GetFileSizeEx(FileHandle, &FileSize))
        {
            Result.ContentSize = (uint32)FileSize.QuadPart;
            Result.Content = VirtualAlloc(0, FileSize.QuadPart, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
            if(Result.Content)
            {
                DWORD ReadBytes;
                if(!::ReadFile(FileHandle, Result.Content, Result.ContentSize, &ReadBytes, 0) && (Result.ContentSize == ReadBytes))
                {
                    WIN_LOG(Warn, "File Read Error: %s", Filename);
                    Result.Content = nullptr;
                }
            }
        }
    }

    return Result;
}
