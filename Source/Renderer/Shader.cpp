#include "Renderer/Shader.h"

#include "Core/FileIO.h"
#include "Core/Logger.h"
#include "Renderer/DX11Renderer.h"

Shader::Shader(ID3D11Device* Device, const ShaderType InType, const char* FilePath)
    : ShaderContents(nullptr)
    ,ShaderSize(0)
{
    Type = InType;
    
    const ReadFileResult Result = FileIO::ReadFile(FilePath);
    if (Result.IsValid())
    {
        ShaderContents = Result.Content;
        ShaderSize = Result.ContentSize;
        if (Device)
        {
            switch(InType)
            {
                case ShaderType::Vertex: HR_CHECK(Device->CreateVertexShader(Result.Content, Result.ContentSize, nullptr, &VertexShader)); break;
                case ShaderType::Pixel: HR_CHECK(Device->CreatePixelShader(Result.Content, Result.ContentSize, nullptr, &PixelShader)); break;
            }
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
