#include "Renderer/Texture.h"

Texture::Texture(const ImageData& InData)
    : TextureShaderResource(nullptr)
{
    Data = InData;
}

