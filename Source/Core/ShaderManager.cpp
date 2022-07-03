#include "Core/ShaderManager.h"

#include "Core/FileIO.h"
#include "Core/Logger.h"
#include "Renderer/Renderer.h"

IRenderer* ShaderManager::Renderer = nullptr;
HashMap<const char*, Shader*> ShaderManager::ShaderMap;

void ShaderManager::Initialize(IRenderer* InRenderer)
{
    Renderer = InRenderer;
}

void ShaderManager::Shutdown()
{
    ShaderMap.Empty([](const HashMapNode<const char*, Shader*>& Element)
    {
        Renderer->DestroyShader(Element.GetValue());
    });
}

Shader* ShaderManager::Create(ShaderType InType, const char* InFilepath)
{
    const ReadFileResult ReadFileResult = FileIO::ReadFile(InFilepath);
    if (ReadFileResult.IsValid())
    {
        Shader* Result = new Shader(InType, ReadFileResult.Content, ReadFileResult.ContentSize);
        Renderer->CreateShader(InType, ReadFileResult.Content, ReadFileResult.ContentSize, &Result->ShaderData);
        ShaderMap.Add(InFilepath, Result);
        return Result;
    }
    
    WIN_LOG(Warn, "Failed to Load Shader: %s", InFilepath);
    return nullptr;
}

Shader* ShaderManager::Get(const char* InFilepath)
{
    Shader* ResultShader = nullptr;
    ShaderMap.Find(InFilepath, ResultShader);
    return ResultShader;
}
