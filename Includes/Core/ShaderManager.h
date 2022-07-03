#pragma once
#include "Containers/HashMap.h"
#include "Renderer/Shader.h"

class IRenderer;
class ShaderManager
{
public:
    static void Initialize(IRenderer* InRenderer);
    static void Shutdown();
    
public:
    static Shader* Create(ShaderType InType, const char* InFilepath);
    static Shader* Get(const char* InFilepath);

private:
    static IRenderer* Renderer;
    static HashMap<const char*, Shader*> ShaderMap;
};