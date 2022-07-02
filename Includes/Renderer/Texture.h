#pragma once
#include "Shader.h"

class Texture
{
public:
    // TODO(HO): TextureFactory with Device?
    explicit Texture(const char* InFilepath, ID3D11Device* InDevice);
    ~Texture();
    ID3D11ShaderResourceView* GetResource() const { return ResourceView; }
    
private:
    const char* Filepath;
    int32 Width;
    int32 Height;
    int32 Channels;
    uint64 Size;

private:
    ID3D11Texture2D* D3DTexture;
    ID3D11ShaderResourceView* ResourceView;
};
