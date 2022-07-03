#pragma once

class Window;
class Texture;
class Shader;
enum class ShaderType : uint8;
class IRenderer
{
public:
    virtual ~IRenderer() = default;
    
    virtual bool Initialize(Window* MainWindow) = 0;
    virtual void Shutdown() = 0;

    // TODO(HO): Move SceneManagement
    virtual bool InitializeScene() = 0;
    virtual void UpdateScene(float64 DeltaTime) = 0;
    virtual void RenderScene() = 0;
    virtual void CleanupScene() = 0;

    virtual void PreRender() = 0;
    virtual void PostRender() = 0;

    virtual void CreateTexture(void* InTextureData, uint32 InWidth, uint32 InHeight, void** OutData) = 0;
    virtual void DestroyTexture(Texture* InTexture) = 0;
    
    virtual void CreateShader(ShaderType InType, void* InShaderData, uint64 InSize, void** OutShader) = 0;
    virtual void DestroyShader(Shader* InShader) = 0;

    virtual void ResizeViewport(float Width, float Height) = 0;
};