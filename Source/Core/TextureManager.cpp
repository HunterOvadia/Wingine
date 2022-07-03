#include "Core/TextureManager.h"
#include "Core/Logger.h"
#include "Renderer/DX11Renderer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "ThirdParty/stb_image.h"

IRenderer* TextureManager::Renderer = nullptr;
HashMap<const char*, Texture*> TextureManager::TextureMap;

void TextureManager::Initialize(IRenderer* InRenderer)
{
    Renderer = InRenderer;
}

void TextureManager::Shutdown()
{
    TextureMap.Empty([](const HashMapNode<const char*, Texture*>& Element)
    {
        Renderer->DestroyTexture(Element.GetValue());
    });
}


Texture* TextureManager::Create(const char* InFilepath)
{
    ImageData TextureData = {};
    TextureData.Filepath = InFilepath;

    uint8* Data = stbi_load(InFilepath, &TextureData.Width, &TextureData.Height, &TextureData.Channels, STBI_rgb_alpha);
    if(Data)
    {
        Texture* Result = new Texture(TextureData);
        Renderer->CreateTexture(Data, TextureData.Width, TextureData.Height, &Result->TextureShaderResource);
        SAFE_FREE(Data);

        TextureMap.Add(InFilepath, Result);
        return Result;
    }
    
    WIN_LOG(Warn, "Failed to Load Texture: %s", InFilepath);
    return nullptr;
}

Texture* TextureManager::Get(const char* InFilepath)
{
    Texture* ResultTexture = nullptr;
    TextureMap.Find(InFilepath, ResultTexture);
    return ResultTexture;
}
