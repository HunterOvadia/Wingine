#pragma once
#include "Shader.h"

struct ImageData
{
    const char* Filepath;
    int32 Width;
    int32 Height;
    int32 Channels;
};

class Texture
{
    friend class TextureManager;
    
public:
    explicit Texture(const ImageData& InData);
    template<typename T>
    T* GetResource() const { return static_cast<T*>(TextureShaderResource); }
    
private:
    ImageData Data;

private:
    void* TextureShaderResource;
};
