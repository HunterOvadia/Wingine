#pragma once
#include "Defines.h"

enum class ShaderType : uint8
{
    Vertex,
    Pixel
};

class Shader
{
    friend class ShaderManager;
    
public:
    Shader(ShaderType InType, void* InShaderBytecode, uint64 InShaderSize)
        : Type(InType)
        , ShaderData(nullptr)
        , ShaderBytecode(InShaderBytecode)
        , ShaderSize(InShaderSize) {}
    
    template<typename T>
    T* GetShader() const { return (T*)ShaderData; }
    ShaderType GetType() const { return Type; }
    void* GetBytecode() const { return ShaderBytecode; }
    uint64 GetSize() const { return ShaderSize; }
    
private:
    ShaderType Type;
    void* ShaderData;
    void* ShaderBytecode;
    uint64 ShaderSize;
};