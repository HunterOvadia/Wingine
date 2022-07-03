#include "Renderer/DX11Renderer.h"
#include <d3d11.h>
#include "Core/FileIO.h"
#include "Core/Logger.h"
#include "Core/ShaderManager.h"
#include "Core/TextureManager.h"
#include "Renderer/Shader.h"
#include "Renderer/Vertex.h"

void CameraData::Initialize(const uint32 Width, const uint32 Height)
{
    Position = DirectX::XMVectorSet(0.0f, 3.0f, -8.0f, 0.0f);
    Target = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    Up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    View = DirectX::XMMatrixLookAtLH(Position, Target, Up);
    Projection = DirectX::XMMatrixPerspectiveFovLH(0.4f * 3.14f, static_cast<float>(Width)/static_cast<float>(Height), 1.0f, 1000.0f);
}

DX11Renderer::DX11Renderer()
    : SwapChain(nullptr)
      , Device(nullptr)
      , DeviceContext(nullptr)
      , RenderTargetView(nullptr)
      , DepthStencilView(nullptr)
      , DepthStencilBuffer(nullptr)
      , ConstantBuffer(nullptr)
      , WireFrameRasterizerState(nullptr)
      , SquareIndexBuffer(nullptr)
      , SquareVertexBuffer(nullptr)
      , VertexInputLayout(nullptr)
      , PixelShader(nullptr)
      , VertexShader(nullptr)
      , CubeTexture(nullptr), TextureSamplerState(nullptr), Width(0)
      , Height(0)
      , Camera()
      , ConstantBufferData()
      , Cube1World()
      , Cube2World()
{
}

bool DX11Renderer::Initialize(Window* MainWindow)
{
    CreateDeviceAndSwapChain(MainWindow);
    CreateRenderTargetView();
    CreateDepthStencilView();
    
    DeviceContext->OMSetRenderTargets(1, &RenderTargetView, DepthStencilView);
    return true;
}

void DX11Renderer::Shutdown()
{
    DX_SAFE_RELEASE(WireFrameRasterizerState);
    DX_SAFE_RELEASE(ConstantBuffer);
    DX_SAFE_RELEASE(VertexInputLayout);
    DX_SAFE_RELEASE(SquareIndexBuffer);
    DX_SAFE_RELEASE(SquareVertexBuffer);
    DX_SAFE_RELEASE(DepthStencilView);
    DX_SAFE_RELEASE(DepthStencilBuffer);
    DX_SAFE_RELEASE(RenderTargetView);
    DX_SAFE_RELEASE(SwapChain);
    DX_SAFE_RELEASE(Device);
    DX_SAFE_RELEASE(DeviceContext);
}

void DX11Renderer::PreRender()
{
    ClearViews();
}

void DX11Renderer::SetConstantBufferData(const DirectX::XMMATRIX& InWVP)
{
    ConstantBufferData.UpdateData(InWVP, Camera);
    DeviceContext->UpdateSubresource(ConstantBuffer, 0, nullptr, &ConstantBufferData, 0, 0);
    DeviceContext->VSSetConstantBuffers(0, 1, &ConstantBuffer);
}

void DX11Renderer::CreateTexture(void* InTextureData, uint32 InWidth, uint32 InHeight, void** OutData)
{
    const D3D11_TEXTURE2D_DESC TextureDesc =
{
        .Width = InWidth,
        .Height = InHeight,
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
        .pSysMem = InTextureData,
        .SysMemPitch = (InWidth * 4)
    };

    ID3D11Texture2D* Texture;
    HR_CHECK(Device->CreateTexture2D(&TextureDesc, &TextureData, &Texture));
    HR_CHECK(Device->CreateShaderResourceView(Texture, nullptr, reinterpret_cast<ID3D11ShaderResourceView**>(OutData)));
    DX_SAFE_RELEASE(Texture);
}

void DX11Renderer::DestroyTexture(Texture* InTexture)
{
    ID3D11ShaderResourceView* TexSRV = InTexture->GetResource<ID3D11ShaderResourceView>();
    DX_SAFE_RELEASE(TexSRV);
}

void DX11Renderer::CreateShader(ShaderType InType, void* InShaderData, uint64 InSize, void** OutShader)
{
    switch(InType)
    {
        case ShaderType::Vertex:
        {
           HR_CHECK(Device->CreateVertexShader(InShaderData, InSize, nullptr, reinterpret_cast<ID3D11VertexShader**>(OutShader)));
        }
        break;
        case ShaderType::Pixel:
        {
           HR_CHECK(Device->CreatePixelShader(InShaderData, InSize, nullptr, reinterpret_cast<ID3D11PixelShader**>(OutShader)));
        }
        break;
    }
}

void DX11Renderer::DestroyShader(Shader* InShader)
{
    switch(InShader->GetType())
    {
        case ShaderType::Vertex:
        {
            ID3D11VertexShader* VS = InShader->GetShader<ID3D11VertexShader>();
            DX_SAFE_RELEASE(VS);
        }
        break;
        case ShaderType::Pixel:
        {
            ID3D11PixelShader* PS = InShader->GetShader<ID3D11PixelShader>();
            DX_SAFE_RELEASE(PS);
        }
        break;
    }

    if(void* ByteCode = InShader->GetBytecode())
    {
        VirtualFree(ByteCode, InShader->GetSize(), MEM_RELEASE);
        ByteCode = nullptr;
    }
}

void DX11Renderer::SetTexture(ID3D11ShaderResourceView* ResourceView, ID3D11SamplerState* SamplerState)
{
    DeviceContext->PSSetShaderResources(0, 1, &ResourceView);
    DeviceContext->PSSetSamplers(0, 1, &SamplerState);
}

bool DX11Renderer::InitializeScene()
{
    if(!InitializeTextures() || !InitializeShaders())
    {
        return false;
    }

    SetShader(VertexShader);
    SetShader(PixelShader);
    
    CreateSamplerState();
    CreateVertexBuffer();
    CreateIndexBuffer();
    CreateConstantBuffer();
    CreateInputLayout();
    
    // TODO(HO): States?
    CreateWireframeRasterizerState();
    
    CreateViewport(static_cast<float>(Width), static_cast<float>(Height));

    // TODO(HO): Move this :)
    Camera.Initialize(Width, Height);

    
    DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // TODO(HO): States?
    //DeviceContext->RSSetState(WireFrameRasterizerState);
    return true;
}

void DX11Renderer::UpdateScene()
{
    Rot += 0.0005f;
    if(Rot > 6.28f)
    {
        Rot = 0.0f;
    }

    Cube1World = DirectX::XMMatrixIdentity();

    const DirectX::XMVECTOR RotAxis = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    DirectX::XMMATRIX Rotation = DirectX::XMMatrixRotationAxis(RotAxis, Rot);
    const DirectX::XMMATRIX Translation = DirectX::XMMatrixTranslation(0.0f, 0.0f, 4.0f);
    Cube1World = (Translation * Rotation);

    Cube2World = DirectX::XMMatrixIdentity();
    Rotation = DirectX::XMMatrixRotationAxis(RotAxis, -Rot);
    const DirectX::XMMATRIX Scale = DirectX::XMMatrixScaling(1.3f, 1.3f, 1.3f);
    Cube2World = (Rotation * Scale);
}


void DX11Renderer::RenderScene()
{
    SetConstantBufferData(Cube1World);
    SetTexture(CubeTexture->GetResource<ID3D11ShaderResourceView>(), TextureSamplerState);
    DeviceContext->DrawIndexed(36, 0, 0);

    SetConstantBufferData(Cube2World);
    SetTexture(CubeTexture->GetResource<ID3D11ShaderResourceView>(), TextureSamplerState);
    DeviceContext->DrawIndexed(36, 0, 0);
}

void DX11Renderer::CleanupScene()
{
}

void DX11Renderer::PostRender()
{
    SwapChain->Present(1, 0);
}

void DX11Renderer::CreateVertexBuffer()
{
    const Vertex VertexBuffer[] =
    {
        Vertex(Vector3(-1.0f, -1.0f, -1.0f), Vector2(0.0f, 1.0f)),
        Vertex(Vector3(-1.0f,  1.0f, -1.0f), Vector2(0.0f, 0.0f)),
        Vertex(Vector3( 1.0f,  1.0f, -1.0f), Vector2(1.0f, 0.0f)),
        Vertex(Vector3( 1.0f, -1.0f, -1.0f), Vector2(1.0f, 1.0f)),
        
        Vertex(Vector3(-1.0f, -1.0f, 1.0f), Vector2(1.0f, 1.0f)),
        Vertex(Vector3( 1.0f, -1.0f, 1.0f), Vector2(0.0f, 1.0f)),
        Vertex(Vector3( 1.0f,  1.0f, 1.0f), Vector2(0.0f, 0.0f)),
        Vertex(Vector3(-1.0f,  1.0f, 1.0f), Vector2(1.0f, 0.0f)),

        Vertex(Vector3(-1.0f, 1.0f, -1.0f), Vector2(0.0f, 1.0f)),
        Vertex(Vector3(-1.0f, 1.0f,  1.0f), Vector2(0.0f, 0.0f)),
        Vertex(Vector3( 1.0f, 1.0f,  1.0f), Vector2(1.0f, 0.0f)),
        Vertex(Vector3( 1.0f, 1.0f, -1.0f), Vector2(1.0f, 1.0f)),

        Vertex(Vector3(-1.0f, -1.0f, -1.0f), Vector2(1.0f, 1.0f)),
        Vertex(Vector3( 1.0f, -1.0f, -1.0f), Vector2(0.0f, 1.0f)),
        Vertex(Vector3( 1.0f, -1.0f,  1.0f), Vector2(0.0f, 0.0f)),
        Vertex(Vector3(-1.0f, -1.0f,  1.0f), Vector2(1.0f, 0.0f)),

        Vertex(Vector3(-1.0f, -1.0f,  1.0f), Vector2(0.0f, 1.0f)),
        Vertex(Vector3(-1.0f,  1.0f,  1.0f), Vector2(0.0f, 0.0f)),
        Vertex(Vector3(-1.0f,  1.0f, -1.0f), Vector2(1.0f, 0.0f)),
        Vertex(Vector3(-1.0f, -1.0f, -1.0f), Vector2(1.0f, 1.0f)),

        Vertex(Vector3( 1.0f, -1.0f, -1.0f), Vector2(0.0f, 1.0f)),
        Vertex(Vector3( 1.0f,  1.0f, -1.0f), Vector2(0.0f, 0.0f)),
        Vertex(Vector3( 1.0f,  1.0f,  1.0f), Vector2(1.0f, 0.0f)),
        Vertex(Vector3( 1.0f, -1.0f,  1.0f), Vector2(1.0f, 1.0f)),
    };

    const D3D11_BUFFER_DESC VertexBufferDesc =
    {
        .ByteWidth = sizeof(Vertex) * STATIC_ARRAY_SIZE(VertexBuffer),
        .Usage = D3D11_USAGE_DEFAULT,
        .BindFlags = D3D11_BIND_VERTEX_BUFFER,
        .CPUAccessFlags = 0,
        .MiscFlags = 0
    };
    CreateBuffer(&VertexBuffer, &VertexBufferDesc, &SquareVertexBuffer);

    constexpr UINT Stride = sizeof(Vertex);
    constexpr UINT Offset = 0;
    DeviceContext->IASetVertexBuffers(0, 1, &SquareVertexBuffer, &Stride, &Offset);
}

void DX11Renderer::CreateIndexBuffer()
{
    const DWORD Indices[] =
{
        // Front Face
        0,  1,  2,
        0,  2,  3,
    
        // Back Face
        4,  5,  6,
        4,  6,  7,
    
        // Top Face
        8,  9, 10,
        8, 10, 11,
    
        // Bottom Face
        12, 13, 14,
        12, 14, 15,
    
        // Left Face
        16, 17, 18,
        16, 18, 19,
    
        // Right Face
        20, 21, 22,
        20, 22, 23
    };

    const D3D11_BUFFER_DESC IndexBufferDesc =
    {
        .ByteWidth = sizeof(DWORD) * STATIC_ARRAY_SIZE(Indices) * 3,
        .Usage = D3D11_USAGE_DEFAULT,
        .BindFlags = D3D11_BIND_INDEX_BUFFER,
        .CPUAccessFlags = 0,
        .MiscFlags = 0,
    };

    CreateBuffer(&Indices, &IndexBufferDesc, &SquareIndexBuffer);
    DeviceContext->IASetIndexBuffer(SquareIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
}


void DX11Renderer::CreateConstantBuffer()
{
    const D3D11_BUFFER_DESC ConstantBufferDesc =
    {
        .ByteWidth = sizeof(ConstantBufferPerObjectData),
        .Usage = D3D11_USAGE_DEFAULT,
        .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
        .CPUAccessFlags = 0,
        .MiscFlags = 0
    };
    CreateBuffer(nullptr, &ConstantBufferDesc, &ConstantBuffer);
}

void DX11Renderer::CreateInputLayout()
{
    HR_CHECK(Device->CreateInputLayout(Vertex::Layout, Vertex::LayoutCount, VertexShader->GetBytecode(), VertexShader->GetSize(), &VertexInputLayout));
    DeviceContext->IASetInputLayout(VertexInputLayout);
}

void DX11Renderer::SetShader(const Shader* InShader) const
{
    if(!InShader)
    {
        return;
    }
    
    switch(InShader->GetType())
    {
        case ShaderType::Vertex:
        {
            DeviceContext->VSSetShader(InShader->GetShader<ID3D11VertexShader>(), nullptr, 0);
        }
        break;
        case ShaderType::Pixel:
        {
            DeviceContext->PSSetShader(InShader->GetShader<ID3D11PixelShader>(), nullptr, 0);
        }
        break;
    }
}

void DX11Renderer::CreateDeviceAndSwapChain(Window* MainWindow)
{
    const WindowSettings& WindowSettings = MainWindow->GetSettings();
    Width = WindowSettings.Size.X;
    Height = WindowSettings.Size.Y;
    
    DXGI_MODE_DESC BufferDesc =
    {
        .Width = Width,
        .Height = Height,
        
        .RefreshRate = {
            .Numerator = 60,
            .Denominator = 1
         },
        
        .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
        .ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
        .Scaling = DXGI_MODE_SCALING_UNSPECIFIED
    };

    const DXGI_SWAP_CHAIN_DESC SwapChainDesc =
    {
        .BufferDesc = BufferDesc,
        .SampleDesc = {
            .Count = 1,
            .Quality = 0
         },
        .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
        .BufferCount = 1,
        .OutputWindow = MainWindow->GetHandle(),
        .Windowed = true,
        .SwapEffect = DXGI_SWAP_EFFECT_DISCARD
    };

    HR_CHECK(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, NULL, nullptr, NULL, D3D11_SDK_VERSION, &SwapChainDesc, &SwapChain, &Device, nullptr, &DeviceContext));
}

void DX11Renderer::CreateRenderTargetView()
{
    ID3D11Texture2D* BackBuffer;
    HR_CHECK(SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&BackBuffer)));
    HR_CHECK(Device->CreateRenderTargetView(BackBuffer, nullptr, &RenderTargetView));
    DX_SAFE_RELEASE(BackBuffer);
}

void DX11Renderer::CreateDepthStencilView()
{
    const D3D11_TEXTURE2D_DESC DepthStencilDesc =
    {
        .Width = Width,
        .Height = Height,
        .MipLevels = 1,
        .ArraySize = 1,
        .Format = DXGI_FORMAT_D24_UNORM_S8_UINT,
        .SampleDesc = {
            .Count = 1,
            .Quality = 0,
        },
        .Usage = D3D11_USAGE_DEFAULT,
        .BindFlags = D3D11_BIND_DEPTH_STENCIL,
        .CPUAccessFlags = 0,
        .MiscFlags = 0
    };

    HR_CHECK(Device->CreateTexture2D(&DepthStencilDesc, nullptr, &DepthStencilBuffer));
    HR_CHECK(Device->CreateDepthStencilView(DepthStencilBuffer, nullptr, &DepthStencilView));
}

bool DX11Renderer::InitializeShaders()
{
    VertexShader = ShaderManager::Create(ShaderType::Vertex, "VertexShader.cso");
    PixelShader = ShaderManager::Create(ShaderType::Pixel, "PixelShader.cso");
    return true;
}

bool DX11Renderer::InitializeTextures()
{
    CubeTexture = TextureManager::Create("TestTexture.png");
    return true;
}

void DX11Renderer::CreateSamplerState()
{
    const D3D11_SAMPLER_DESC SamplerDesc =
    {
        .Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
        .AddressU = D3D11_TEXTURE_ADDRESS_WRAP,
        .AddressV = D3D11_TEXTURE_ADDRESS_WRAP,
        .AddressW = D3D11_TEXTURE_ADDRESS_WRAP,
        .ComparisonFunc = D3D11_COMPARISON_NEVER,
        .MinLOD = 0,
        .MaxLOD = D3D11_FLOAT32_MAX
    };
    
    HR_CHECK(Device->CreateSamplerState(&SamplerDesc, &TextureSamplerState));
}


void DX11Renderer::CreateBuffer(const void* InBufferMemory, const D3D11_BUFFER_DESC* InBufferDesc, ID3D11Buffer** InBuffer) const
{
    const D3D11_SUBRESOURCE_DATA BufferData
    {
        .pSysMem = InBufferMemory,
    };
    
    HR_CHECK(Device->CreateBuffer(InBufferDesc, InBufferMemory ? &BufferData : nullptr, InBuffer));
}

void DX11Renderer::CreateViewport(float Width, float Height) const
{
    const D3D11_VIEWPORT Viewport =
    {
        .TopLeftX = 0,
        .TopLeftY = 0,
        .Width = Width,
        .Height = Height,
        .MinDepth = 0.0f,
        .MaxDepth = 1.0f
    };
    
    DeviceContext->RSSetViewports(1, &Viewport);
}

void DX11Renderer::CreateWireframeRasterizerState()
{
    const D3D11_RASTERIZER_DESC RasterizerDesc =
    {
        .FillMode = D3D11_FILL_WIREFRAME,
        .CullMode = D3D11_CULL_NONE
    };

    HR_CHECK(Device->CreateRasterizerState(&RasterizerDesc, &WireFrameRasterizerState));
}

void DX11Renderer::ClearViews()
{
    constexpr FLOAT Colors[4] = { 0.0f, 0.0, 0.0f, 1.0f };
    DeviceContext->ClearRenderTargetView(RenderTargetView, Colors);
    DeviceContext->ClearDepthStencilView(DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}
