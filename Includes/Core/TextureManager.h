#pragma once
#include "Containers/HashMap.h"
#include "Renderer/Texture.h"

class IRenderer;
class TextureManager
{
public:
    static void Initialize(IRenderer* InRenderer);
    static void Shutdown();
    
public:
    static Texture* Create(const char* InFilepath);
    static Texture* Get(const char* InFilepath);

private:
    static IRenderer* Renderer;
    static HashMap<const char*, Texture*> TextureMap;
};
