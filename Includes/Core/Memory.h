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
    Memory()
        : Head(nullptr)
        , TotalAllocationSize(0) {}
    
    void* Allocate(uint64 InSize);
    void Free(void* Pointer);
    void Set(void* Dest, int Value, uint64 Size);
    void Zero(void* Dest, uint64 Size);
    void Log() const;

private:
    MemoryBlockNode* Head;
    uint64 TotalAllocationSize;
};

static Memory GMemory;