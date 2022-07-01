#include "Renderer/DX11Renderer.h"
#include <d3d11.h>

#include "Core/FileIO.h"
#include "Core/Logger.h"
#include "Renderer/ShaderBase.h"

DX11Renderer::DX11Renderer()
    : SwapChain(nullptr)
    , Device(nullptr)
    , DeviceContext(nullptr)
    , RenderTargetView(nullptr)
    , DepthStencilView(nullptr)
    , DepthStencilBuffer(nullptr)
    , SquareIndexBuffer(nullptr)
    , SquareVertexBuffer(nullptr)
    , VertexInputLayout(nullptr)
    , PixelShader(nullptr)
    , VertexShader(nullptr)
    , Width(0)
    , Height(0)
{
}

bool DX11Renderer::Initialize(Window* MainWindow)
{
    CreateDeviceAndSwapChain(MainWindow);
    CreateRenderTargetView();
    CreateDepthStencilView();
    
    DeviceContext->OMSetRenderTargets(1, &RenderTargetView, DepthStencilView);
    return SetupScene();
}

void DX11Renderer::Shutdown()
{
    DX_SAFE_RELEASE(VertexInputLayout);
    DX_SAFE_RELEASE(SquareIndexBuffer);
    DX_SAFE_RELEASE(SquareVertexBuffer);

    delete PixelShader;
    delete VertexShader;

    DX_SAFE_RELEASE(DepthStencilView);
    DX_SAFE_RELEASE(DepthStencilBuffer);
    DX_SAFE_RELEASE(RenderTargetView);
    DX_SAFE_RELEASE(SwapChain);
    DX_SAFE_RELEASE(Device);
    DX_SAFE_RELEASE(DeviceContext);
}

void DX11Renderer::PreRender() const
{
    constexpr FLOAT Colors[4] = { 0.5f, 0.3f, 0.3f, 1.0f };
    DeviceContext->ClearRenderTargetView(RenderTargetView, Colors);
    DeviceContext->ClearDepthStencilView(DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void DX11Renderer::Render() const
{
    PreRender();

    // Rendering
    DeviceContext->DrawIndexed(6, 0, 0);

    PostRender();
}

bool DX11Renderer::SetupScene()
{
    if(!InitializeShaders())
    {
        return false;
    }

    SetShaders(VertexShader, PixelShader);

    const Vertex VertexBuffer[] =
    {
        Vertex(Vector3(-0.5f, -0.5f, 0.5f), Vector4(1.0f, 0.0f, 0.0f, 1.0f)),
        Vertex(Vector3(-0.5f, 0.5f, 0.5f), Vector4(0.0f, 1.0f, 0.0f, 1.0f)),
        Vertex(Vector3(0.5f, 0.5f, 0.5f), Vector4(0.0f, 0.0f, 1.0f, 1.0f)),
        Vertex(Vector3(0.5f, -0.5f, 0.5f), Vector4(0.0f, 1.0f, 0.0f, 1.0f)),
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

    const DWORD Indices[] =
    {
        0, 1, 2,
        0, 2, 3
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


    constexpr UINT Stride = sizeof(Vertex);
    constexpr UINT Offset = 0;
    DeviceContext->IASetVertexBuffers(0, 1, &SquareVertexBuffer, &Stride, &Offset);
    DeviceContext->IASetIndexBuffer(SquareIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    HR_CHECK(Device->CreateInputLayout(Vertex::Layout, Vertex::LayoutCount, VertexShader->GetContents(), VertexShader->GetSize(), &VertexInputLayout));
    DeviceContext->IASetInputLayout(VertexInputLayout);
    DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    
    const D3D11_VIEWPORT Viewport =
    {
        .TopLeftX = 0,
        .TopLeftY = 0,
        .Width = static_cast<float>(Width),
        .Height = static_cast<float>(Height),
        .MinDepth = 0.0f,
        .MaxDepth = 1.0f
    };
    
    DeviceContext->RSSetViewports(1, &Viewport);
    return true;
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
            DeviceContext->VSSetShader(static_cast<ID3D11VertexShader*>(InShader->GetShader()), nullptr, 0);
        }
        break;
        case ShaderType::Pixel:
        {
            DeviceContext->PSSetShader(static_cast<ID3D11PixelShader*>(InShader->GetShader()), nullptr, 0);
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
    HR_CHECK(SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&BackBuffer));
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
    const ReadFileResult VertexShaderResult = FileIO::ReadFile("VertexShader.cso");
    if(!VertexShaderResult.IsValid())
    {
        return false;
    }

    const ReadFileResult PixelShaderResult = FileIO::ReadFile("PixelShader.cso");
    if(!VertexShaderResult.IsValid())
    {
        return false;
    }

    // TODO(HO): Shader Management :)
    VertexShader = new Shader(Device, ShaderType::Vertex, VertexShaderResult.Content, VertexShaderResult.ContentSize);
    PixelShader = new Shader(Device, ShaderType::Pixel, PixelShaderResult.Content, PixelShaderResult.ContentSize);
    return true;
}

void DX11Renderer::SetShaders(Shader* InVertexShader, Shader* InPixelShader) const
{
    if(InVertexShader)
    {
        SetShader(VertexShader);
    }

    if(InPixelShader)
    {
        SetShader(PixelShader);
    }
}

void DX11Renderer::CreateBuffer(const void* InBufferMemory, const D3D11_BUFFER_DESC* InBufferDesc, ID3D11Buffer** InBuffer) const
{
    const D3D11_SUBRESOURCE_DATA BufferData
    {
        .pSysMem = InBufferMemory,
    };
    
    HR_CHECK(Device->CreateBuffer(InBufferDesc, &BufferData, InBuffer));
}

void DX11Renderer::PostRender() const
{
    SwapChain->Present(0, 0);
}
