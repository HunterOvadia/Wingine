#include "Core/Memory.h"
#include "Core/Logger.h"
#include <cstdlib>
#include <cstring>


void* Memory::Allocate(const uint64 InSize)
{
    MemoryBlockNode* Node = static_cast<MemoryBlockNode*>(malloc(sizeof(MemoryBlockNode) + InSize));
    if(Node)
    {
        Zero(Node, sizeof(*Node));
    }
    else
    {
        WIN_LOG(Error, "Failed to Allocate Memory");
        return nullptr;
    }

    Node->AllocationSize = InSize;
    TotalAllocationSize += InSize;
    Head = Node;

    return (reinterpret_cast<char*>(Node) + sizeof(*Node));
}

void Memory::Free(void* Pointer)
{
    if(Pointer == nullptr)
    {
        return;
    }
        
    MemoryBlockNode* Node = reinterpret_cast<MemoryBlockNode*>(static_cast<char*>(Pointer) - sizeof(MemoryBlockNode));
    if(Node == Head)
    {
        Head = Node->Next;
    }

    if(Node->Prev != nullptr)
    {
        Node->Prev->Next = Node->Next;
    }

    if(Node->Next != nullptr)
    {
        Node->Next->Prev = Node->Prev;
    }

    TotalAllocationSize -= Node->AllocationSize;
    free(Node);
    Node = nullptr;
}

void Memory::Set(void* Dest, const int Value, const uint64 Size)
{
    memset(Dest, Value, Size);
}

void Memory::Zero(void* Dest, const uint64 Size)
{
    Set(Dest, 0, Size);
}

void Memory::Log() const
{
    MemoryBlockNode* Node = Head;
    while(Node != nullptr)
    {
        WIN_LOG(Error, "Unfreed Block: %p - Size: %lu", (reinterpret_cast<char*>(Node) + sizeof(*Node)), Node->AllocationSize);
        Node = Node->Next;
    }

    if(TotalAllocationSize > 0)
    {
        WIN_LOG(Error, "Application shutdown with %lu byte(s) of unfreed memory.", TotalAllocationSize);
    }
}
