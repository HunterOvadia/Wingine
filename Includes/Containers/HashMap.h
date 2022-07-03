#pragma once
#include "Defines.h"
#include "Core/Application.h"

template<typename K, typename V>
class HashMapNode
{
public:
    HashMapNode(const K& InKey, const V& InValue)
        : Key(InKey)
        , Value(InValue)
        , Next(nullptr) {}

    HashMapNode(const HashMapNode&) = delete;
    HashMapNode& operator=(const HashMapNode&) = delete;

    const K& GetKey() const
    {
        return Key;
    }

    const V& GetValue() const
    {
        return Value;
    }

    void SetValue(const V& InValue)
    {
        Value = InValue;
    }

    HashMapNode* GetNext() const
    {
        return Next;
    }

    void SetNext(HashMapNode* InNext)
    {
        Next = InNext;
    }

private:
    K Key;
    V Value;
    HashMapNode* Next;
};

template<typename K, uint64 TableSize>
struct HashMapKeyHash
{
    uint64 operator()(const K& InKey) const
    {
        return reinterpret_cast<uint64>(InKey) % TableSize;
    }
};

template<typename K, typename V, uint64 TableSize = 1, typename Hash = HashMapKeyHash<K, TableSize>>
class HashMap
{
public:
    using Node = HashMapNode<K, V>;

    HashMap()
        : Table()
        , HashFunction() {}
    
    HashMap(const HashMap& Other) = delete;
    const HashMap& operator=(const HashMap& Other) = delete;

    ~HashMap()
    {
        Empty({});
    }

    void ForEach(FunctionArg<void(const Node&)> Function)
    {
        for(uint64 Index = 0; Index < TableSize; ++Index)
        {
            Node* Entry = GetNode(Index);
            while(Entry != nullptr)
            {
                Function(*Entry);
                Entry = Entry->GetNext();
            }
        }
    }
    
    void Empty(Optional<FunctionArg<void(const Node&)>> Function)
    {
        for(uint64 Index = 0; Index < TableSize; ++Index)
        {
            Node* Entry = GetNode(Index);
            while(Entry != nullptr)
            {
                Node* Prev = Entry;
                Entry = Entry->GetNext();
                if(Function.has_value())
                {
                    Function.value()(*Prev);
                }
                
                SAFE_DELETE(Prev);
            }

            Table[Index] = nullptr;
        }
    }

    bool Find(const K& InKey, V& OutValue)
    {
        Node* Entry = GetNode(InKey);
        while(Entry != nullptr)
        {
            if(Entry->GetKey() == InKey)
            {
                OutValue = Entry->GetValue();
                return true;
            }

            Entry = Entry->GetNext();
        }

        return false;
    }

    void Add(const K& InKey, const V& InValue)
    {
        Node* Prev = nullptr;
        uint64 HashValue = GetHashValue(InKey);
        Node* Entry = GetNode(HashValue);

        while(Entry != nullptr && Entry->GetKey() != InKey)
        {
            Prev = Entry;
            Entry = Entry->GetNext();
        }

        if(Entry == nullptr)
        {
            Entry = new Node(InKey, InValue);
            if(Prev == nullptr)
            {
                Table[HashValue] = Entry;
            }
            else
            {
                Prev->SetNext(Entry);
            }
        }
        else
        {
            Entry->SetValue(InValue);
        }
    }

    void Remove(const K& InKey)
    {
        Node* Prev = nullptr;
        uint64 HashValue = GetHashValue(InKey);
        Node* Entry = GetNode(HashValue);

        while(Entry != nullptr && Entry->GetKey() != InKey)
        {
            Prev = Entry;
            Entry = Entry->GetNext();
        }

        if(Entry == nullptr)
        {
           return;
        }

        if(Prev == nullptr)
        {
            Table[HashValue] = Entry->GetNext();
        }
        else
        {
            Prev->SetNext(Entry->GetNext());
        }

        SAFE_DELETE(Entry);
    }

private:
    Node* GetNode(const K& InKey)
    {
        const uint64 HashValue = GetHashValue(InKey);
        return GetNode(HashValue);
    }

    Node* GetNode(uint64 InHash)
    {
        return Table[InHash];
    }
    
    uint64 GetHashValue(const K& InKey)
    {
        return HashFunction(InKey);
    }
    
private:
    HashMapNode<K, V>* Table[TableSize];
    Hash HashFunction;
};