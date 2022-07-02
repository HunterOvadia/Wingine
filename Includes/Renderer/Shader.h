#pragma once
#include "Defines.h"
#include "Vertex.h"

enum class ShaderType : uint8
{
    Vertex,
    Pixel
};

class Shader
{
public:
    Shader() = delete;
    Shader(ID3D11Device* Device, ShaderType InType, const char* FilePath);
    ~Shader();

    ShaderType GetType() const { return Type; }
    ID3D11DeviceChild* GetShader() const;
    const void* GetContents() const { return ShaderContents; }
    uint64 GetSize() const { return ShaderSize; }
    
private:
    ShaderType Type;
    ID3D11VertexShader* VertexShader;
    ID3D11PixelShader* PixelShader;
    const void* ShaderContents;
    uint64 ShaderSize;
};