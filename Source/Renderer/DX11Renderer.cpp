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
{
}

bool DX11Renderer::Initialize(Window* MainWindow)
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
    
    ID3D11Texture2D* BackBuffer;
    HR_CHECK(SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&BackBuffer));
    HR_CHECK(Device->CreateRenderTargetView(BackBuffer, nullptr, &RenderTargetView));
    DX_SAFE_RELEASE(BackBuffer);

    DeviceContext->OMSetRenderTargets(1, &RenderTargetView, nullptr);

    return SetupScene();
}

void DX11Renderer::Shutdown()
{
    DX_SAFE_RELEASE(VertexInputLayout);
    DX_SAFE_RELEASE(SquareIndexBuffer);
    DX_SAFE_RELEASE(SquareVertexBuffer);

    delete PixelShader;
    delete VertexShader;

    DX_SAFE_RELEASE(RenderTargetView);
    DX_SAFE_RELEASE(SwapChain);
    DX_SAFE_RELEASE(Device);
    DX_SAFE_RELEASE(DeviceContext);
}

void DX11Renderer::PreRender()
{
    constexpr FLOAT Colors[4] = { 0.5f, 0.3f, 0.3f, 1.0f };
    DeviceContext->ClearRenderTargetView(RenderTargetView, Colors);
}

void DX11Renderer::Render()
{
    PreRender();

    // Rendering
    DeviceContext->DrawIndexed(6, 0, 0);

    PostRender();
}

bool DX11Renderer::SetupScene()
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
    SetShader(VertexShader);
    SetShader(PixelShader);

    Vertex VertexBuffer[] =
    {
        Vertex(Vector3(-0.5f, -0.5f, 0.5f), Vector4(1.0f, 0.0f, 0.0f, 1.0f)),
        Vertex(Vector3(-0.5f, 0.5f, 0.5f), Vector4(0.0f, 1.0f, 0.0f, 1.0f)),
        Vertex(Vector3(0.5f, 0.5f, 0.5f), Vector4(0.0f, 0.0f, 1.0f, 1.0f)),
        Vertex(Vector3(0.5f, -0.5f, 0.5f), Vector4(0.0f, 1.0f, 0.0f, 1.0f)),
    };

    const D3D11_BUFFER_DESC VertexBufferDesc =
{
        .ByteWidth = sizeof(Vertex) * 4,
        .Usage = D3D11_USAGE_DEFAULT,
        .BindFlags = D3D11_BIND_VERTEX_BUFFER,
        .CPUAccessFlags = 0,
        .MiscFlags = 0
    };

    const D3D11_SUBRESOURCE_DATA VertexBufferData
    {
        .pSysMem = VertexBuffer,
    };
    
    HR_CHECK(Device->CreateBuffer(&VertexBufferDesc, &VertexBufferData, &SquareVertexBuffer));

    DWORD Indices[] = {
        0, 1, 2,
        0, 2, 3
    };

    const D3D11_BUFFER_DESC IndexBufferDesc =
    {
        .ByteWidth = sizeof(DWORD) * 2 * 3,
        .Usage = D3D11_USAGE_DEFAULT,
        .BindFlags = D3D11_BIND_INDEX_BUFFER,
        .CPUAccessFlags = 0,
        .MiscFlags = 0,
    };
    
    const D3D11_SUBRESOURCE_DATA IndexBufferData
    {
        .pSysMem = Indices,
    };

    HR_CHECK(Device->CreateBuffer(&IndexBufferDesc, &IndexBufferData, &SquareIndexBuffer));


    constexpr UINT Stride = sizeof(Vertex);
    constexpr UINT Offset = 0;
    DeviceContext->IASetVertexBuffers(0, 1, &SquareVertexBuffer, &Stride, &Offset);
    DeviceContext->IASetIndexBuffer(SquareIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    HR_CHECK(Device->CreateInputLayout(Vertex::Layout, Vertex::LayoutCount, VertexShaderResult.Content, VertexShaderResult.ContentSize, &VertexInputLayout));
    DeviceContext->IASetInputLayout(VertexInputLayout);
    DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    
    const D3D11_VIEWPORT Viewport =
    {
        .TopLeftX = 0,
        .TopLeftY = 0,
        .Width = static_cast<float>(Width),
        .Height = static_cast<float>(Height)
    };
    
    DeviceContext->RSSetViewports(1, &Viewport);
    return true;
}

void DX11Renderer::SetShader(const Shader* InShader) const
{
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

void DX11Renderer::PostRender()
{
    SwapChain->Present(0, 0);
}
