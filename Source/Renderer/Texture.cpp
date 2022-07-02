#include "Renderer/Texture.h"
#include "Renderer/DX11Renderer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "Core/Logger.h"
#include "ThirdParty/stb_image.h"

Texture::Texture(const char* InFilepath, ID3D11Device* InDevice)
    : Size(0)
    , D3DTexture(nullptr)
    , ResourceView(nullptr)
{
    Filepath = InFilepath;
    if (InFilepath)
    {
        uint8* Data = stbi_load(InFilepath, &Width, &Height, &Channels, STBI_rgb_alpha);
        if(Data)
        {
            const D3D11_TEXTURE2D_DESC TextureDesc =
            {
                .Width = static_cast<uint32>(Width),
                .Height = static_cast<uint32>(Height),
                .MipLevels = 1,
                .ArraySize = 1,
                .Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
                .SampleDesc = {
                    .Count = 1,
                    .Quality = 0,
                },
                .Usage = D3D11_USAGE_IMMUTABLE,
                .BindFlags = D3D11_BIND_SHADER_RESOURCE,
                .CPUAccessFlags = 0,
                .MiscFlags = 0
            };

            const D3D11_SUBRESOURCE_DATA TextureData =
            {
                .pSysMem = Data,
                .SysMemPitch = (static_cast<uint32>(Width) * 4)
            };
    
            HR_CHECK(InDevice->CreateTexture2D(&TextureDesc, &TextureData, &D3DTexture));
            HR_CHECK(InDevice->CreateShaderResourceView(D3DTexture, nullptr, &ResourceView))
            SAFE_FREE(Data);
        }
        else
        {
            WIN_LOG(Warn, "Failed to Load Texture: %s", InFilepath);
        }
    }
}

Texture::~Texture()
{
    DX_SAFE_RELEASE(D3DTexture);
    DX_SAFE_RELEASE(ResourceView);
}

