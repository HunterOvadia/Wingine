#include "Renderer/ShaderBase.h"
#include "Core/Logger.h"
#include "Renderer/DX11Renderer.h"

Shader::Shader(ID3D11Device* Device, const ShaderType InType, const void* Contents, const uint64 Size)
{
    Type = InType;
    ShaderContents = Contents;
    ShaderSize = Size;
    
    if(Device)
    {
        if(InType == ShaderType::Vertex)
        {
            HR_CHECK(Device->CreateVertexShader(Contents, Size, nullptr, &VertexShader));
        }
        else if(InType == ShaderType::Pixel)
        {
            HR_CHECK(Device->CreatePixelShader(Contents, Size, nullptr, &PixelShader));
        }
    }
}

Shader::~Shader()
{
    DX_SAFE_RELEASE(VertexShader);
    DX_SAFE_RELEASE(PixelShader);
}

ID3D11DeviceChild* Shader::GetShader() const
{
    switch(Type)
    {
        case ShaderType::Vertex: return VertexShader;
        case ShaderType::Pixel: return PixelShader;
    }
    return nullptr;
}
