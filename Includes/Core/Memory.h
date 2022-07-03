#pragma once
#include "Defines.h"

struct MemoryBlockNode
{
    MemoryBlockNode* Prev;
    MemoryBlockNode* Next;
    uint64 AllocationSize;
};

class Memory
{
public:
    static void* Allocate(uint64 InSize);
    static void Free(void* Pointer);
    static void Set(void* Dest, int Value, uint64 Size);
    static void Zero(void* Dest, uint64 Size);
    static void Log();

private:
    static MemoryBlockNode* Head;
    static uint64 TotalAllocationSize;
};