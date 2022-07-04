#include "Renderer/DX11Renderer.h"
#include <d3d11.h>
#include "Core/FileIO.h"
#include "Core/Input.h"
#include "Core/Logger.h"
#include "Core/ShaderManager.h"
#include "Core/TextureManager.h"
#include "Renderer/Shader.h"
#include "Renderer/Vertex.h"


void CameraData::Update(float64 Time)
{
    using namespace DirectX;
    
    const float32 Speed = 15.0f * static_cast<float32>(Time);
    if(Input::IsKeyDown(DIK_W))
    {
        MoveBackForward += Speed;
    }
    if(Input::IsKeyDown(DIK_A))
    {
        MoveLeftRight -= Speed;
    }
    if(Input::IsKeyDown(DIK_S))
    {
        MoveBackForward -= Speed;
    }
    if(Input::IsKeyDown(DIK_D))
    {
        MoveLeftRight += Speed;
    }

     CamYaw += Input::GetMousePosition().X * 0.0001f;
     CamPitch += Input::GetMousePosition().Y * 0.0001f;
    
    CamRotationMatrix = XMMatrixRotationRollPitchYaw(CamPitch, CamYaw, 0.0f);
    Target = XMVector3TransformCoord(DefaultForward, CamRotationMatrix);
    Target = XMVector3Normalize(Target);
    
    const XMMATRIX RotateYTempMatrix = XMMatrixRotationY(CamYaw);
    CamRight = XMVector3TransformCoord(DefaultRight, RotateYTempMatrix);
    Up = XMVector3TransformCoord(Up, RotateYTempMatrix);
    CamForward = XMVector3Transform(DefaultForward, RotateYTempMatrix);
    
    Position += (MoveLeftRight * CamRight);
    Position += (MoveBackForward * CamForward);
    
    MoveLeftRight = 0.0f;
    MoveBackForward = 0.0f;
    Target = (Position + Target);
    
    View = XMMatrixLookAtLH(Position, Target, Up);
}

DX11Renderer::DX11Renderer()
    : SwapChain(nullptr)
      , Device(nullptr)
      , DeviceContext(nullptr)
      , RenderTargetView(nullptr)
      , DepthStencilView(nullptr)
      , DepthStencilBuffer(nullptr)
      , BlendState(nullptr)
      , SamplerState(nullptr)
      , PerObjectConstantBuffer(nullptr)
      , PerFrameConstantBuffer(nullptr)
      , SquareIndexBuffer(nullptr)
      , SquareVertexBuffer(nullptr)
      , VertexInputLayout(nullptr)
      , PixelShader(nullptr)
      , VertexShader(nullptr), CubeTexture(nullptr), Width(0)
      , Height(0)
      , ConstantBufferPerObject(), ConstantBufferPerFrame()
      , WorldLight()
      , Camera()
      , GroundWorld()
{
}

bool DX11Renderer::Initialize(Window* MainWindow)
{
    CreateDeviceAndSwapChain(MainWindow);
    CreateRenderTargetView();
    CreateDepthStencilView();
    CreateSamplerState();
    CreateBlendState();

    CreateVertexBuffer();
    CreateIndexBuffer();
    CreateConstantBuffers();

    DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    InitializeShaders();
    CreateInputLayout();

    ResizeViewport(static_cast<float>(Width), static_cast<float>(Height));
    return true;
}

void DX11Renderer::Shutdown()
{
    DX_SAFE_RELEASE(BlendState);
    DX_SAFE_RELEASE(PerObjectConstantBuffer);
    DX_SAFE_RELEASE(PerFrameConstantBuffer);
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
    DeviceContext->OMSetRenderTargets(1, &RenderTargetView, DepthStencilView);
    ClearViews();
    UpdatePerFrameConstantBuffer();
    SetDefaultBlendState();
}

void DX11Renderer::UpdatePerObjectConstantBuffer(const DirectX::XMMATRIX& InWorld)
{
    ConstantBufferPerObject.UpdateData(InWorld, Camera);
    DeviceContext->UpdateSubresource(PerObjectConstantBuffer, 0, nullptr, &ConstantBufferPerObject, 0, 0);
    DeviceContext->VSSetConstantBuffers(0, 1, &PerObjectConstantBuffer);
}

void DX11Renderer::UpdatePerFrameConstantBuffer()
{
    ConstantBufferPerFrame.UpdateData(WorldLight);
    DeviceContext->UpdateSubresource(PerFrameConstantBuffer, 0, nullptr, &ConstantBufferPerFrame, 0, 0);
    DeviceContext->PSSetConstantBuffers(0, 1, &PerFrameConstantBuffer);
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

void DX11Renderer::SetIndexBuffer(ID3D11Buffer* InBuffer)
{
    DeviceContext->IASetIndexBuffer(InBuffer, DXGI_FORMAT_R32_UINT, 0);
}

void DX11Renderer::SetVertexBuffer(ID3D11Buffer* InBuffer)
{
    constexpr UINT Stride = sizeof(Vertex);
    constexpr UINT Offset = 0;
    DeviceContext->IASetVertexBuffers(0, 1, &InBuffer, &Stride, &Offset);
}

bool DX11Renderer::InitializeScene()
{
    InitializeTextures();
    
    // TODO(HO): Move this
    Camera.Position = DirectX::XMVectorSet(0.0f, 5.0f, -8.0f, 0.0f);
    Camera.Target = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    Camera.Up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    Camera.View = DirectX::XMMatrixLookAtLH(Camera.Position, Camera.Target, Camera.Up);
    Camera.Projection = DirectX::XMMatrixPerspectiveFovLH(0.4f * 3.14f, static_cast<float>(Width)/static_cast<float>(Height), 1.0f, 1000.0f);
    
    WorldLight.Position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
    WorldLight.Range = 100.0f;
    WorldLight.Attenuation = DirectX::XMFLOAT3(0.0f, 0.2f, 0.02f);
    WorldLight.Direction = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
    WorldLight.Ambient = DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
    WorldLight.Diffuse = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    return true;
}

void DX11Renderer::UpdateScene(float64 DeltaTime)
{
    Camera.Update(DeltaTime);

    GroundWorld = DirectX::XMMatrixIdentity();
    const DirectX::XMMATRIX Scale = DirectX::XMMatrixScaling(500.0f, 10.f, 500.0f);
    const DirectX::XMMATRIX Translation = DirectX::XMMatrixTranslation(0.0f, 50.0f, 0.0f);
    GroundWorld = (Scale * Translation);
}

void DX11Renderer::RenderScene()
{
    // Set Shader
    SetShader(VertexShader);
    SetShader(PixelShader);

    // Set Index Buffer
    SetIndexBuffer(SquareIndexBuffer);

    // Set Vertex Buffer
    SetVertexBuffer(SquareVertexBuffer);

    // Set Constant Buffer Per Object
    UpdatePerObjectConstantBuffer(GroundWorld);

    // Set Texture to Sampler
    SetTexture(CubeTexture->GetResource<ID3D11ShaderResourceView>(), SamplerState);

    // Draw
    DeviceContext->DrawIndexed(6, 0, 0);
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
        Vertex(Vector3(-1.0f, -1.0f, -1.0f), Vector2(1000.0f, 100.0f), Vector3(0.0f, 1.0f, 0.0f)),
        Vertex(Vector3( 1.0f, -1.0f, -1.0f), Vector2(  0.0f, 100.0f), Vector3(0.0f, 1.0f, 0.0f)),
        Vertex(Vector3( 1.0f, -1.0f,  1.0f), Vector2(  0.0f,   0.0f), Vector3(0.0f, 1.0f, 0.0f)),
        Vertex(Vector3(-1.0f, -1.0f,  1.0f), Vector2(100.0f,   0.0f), Vector3(0.0f, 1.0f, 0.0f)),
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
}

void DX11Renderer::CreateIndexBuffer()
{
    const DWORD Indices[] =
    {
        0,  1,  2,
        0,  2,  3,
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
}


void DX11Renderer::CreateConstantBuffers()
{
    const D3D11_BUFFER_DESC PerObjectBufferDesc =
    {
        .ByteWidth = sizeof(ConstantBufferPerObject),
        .Usage = D3D11_USAGE_DEFAULT,
        .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
        .CPUAccessFlags = 0,
        .MiscFlags = 0
    };
    CreateBuffer(nullptr, &PerObjectBufferDesc, &PerObjectConstantBuffer);

    const D3D11_BUFFER_DESC PerFrameBufferDesc =
{
        .ByteWidth = sizeof(ConstantBufferPerFrame),
        .Usage = D3D11_USAGE_DEFAULT,
        .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
        .CPUAccessFlags = 0,
        .MiscFlags = 0
    };
    CreateBuffer(nullptr, &PerFrameBufferDesc, &PerFrameConstantBuffer);
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
        .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
    };

    const DXGI_SWAP_CHAIN_DESC SwapChainDesc =
    {
        .BufferDesc = BufferDesc,
        .SampleDesc = {
            .Count = 1,
         },
        .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
        .BufferCount = 2,
        .OutputWindow = MainWindow->GetHandle(),
        .Windowed = true,
        .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
    };

    constexpr uint32 Flags = D3D11_CREATE_DEVICE_DEBUG;
    HR_CHECK(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, Flags, nullptr, NULL, D3D11_SDK_VERSION, &SwapChainDesc, &SwapChain, &Device, nullptr, &DeviceContext));
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
    CubeTexture = TextureManager::Create("Grass.jpg");
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
        .MaxLOD = D3D11_FLOAT32_MAX
    };
    
    HR_CHECK(Device->CreateSamplerState(&SamplerDesc, &SamplerState));
}


void DX11Renderer::CreateBuffer(const void* InBufferMemory, const D3D11_BUFFER_DESC* InBufferDesc, ID3D11Buffer** InBuffer) const
{
    const D3D11_SUBRESOURCE_DATA BufferData
    {
        .pSysMem = InBufferMemory,
    };
    
    HR_CHECK(Device->CreateBuffer(InBufferDesc, InBufferMemory ? &BufferData : nullptr, InBuffer));
}

void DX11Renderer::ResizeViewport(float Width, float Height)
{
    const D3D11_VIEWPORT Viewport =
    {
        .Width = Width,
        .Height = Height,
        .MaxDepth = 1.0f
    };

    if(Viewport.Width > 0 && Viewport.Height > 0)
    {
        DeviceContext->RSSetViewports(1, &Viewport);
    }
}

void DX11Renderer::CreateBlendState()
{
    const D3D11_RENDER_TARGET_BLEND_DESC RenderTargetBlendDesc =
    {
        .BlendEnable = true,
        .SrcBlend = D3D11_BLEND_SRC_COLOR,
        .DestBlend = D3D11_BLEND_BLEND_FACTOR,
        .BlendOp = D3D11_BLEND_OP_ADD,
        .SrcBlendAlpha = D3D11_BLEND_ONE,
        .DestBlendAlpha = D3D11_BLEND_ZERO,
        .BlendOpAlpha = D3D11_BLEND_OP_ADD,
        .RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL
    };

    const D3D11_BLEND_DESC BlendDesc =
    {
        .AlphaToCoverageEnable = false,
        .RenderTarget = {
            RenderTargetBlendDesc
        }
    };

    HR_CHECK(Device->CreateBlendState(&BlendDesc, &BlendState));
}

void DX11Renderer::SetDefaultBlendState()
{
    DeviceContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);
}

void DX11Renderer::ClearViews()
{
    constexpr FLOAT BackgroundColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
    DeviceContext->ClearRenderTargetView(RenderTargetView, BackgroundColor);
    DeviceContext->ClearDepthStencilView(DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}
