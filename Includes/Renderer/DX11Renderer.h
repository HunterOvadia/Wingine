#pragma once
#include <d3d11.h>
#include <dxgi.h>

#include "Core/Application.h"

#define HR_CHECK(HR) if(FAILED(HR)) { WIN_LOG(Error, "DirectX Creation Failure: %s - %d", __FILE__, __LINE__); }
#define DX_SAFE_RELEASE(Com) if(Com) { Com->Release(); Com = nullptr; }

class Shader;

struct RendererInternals
{
    
};

class DX11Renderer
{
public:
    DX11Renderer();
    bool Initialize(Window* MainWindow);
    void Shutdown();
    void Render() const;
    bool SetupScene();
    void SetShader(const Shader* InShader) const;

private:
    void CreateDeviceAndSwapChain(Window* MainWindow);
    void CreateRenderTargetView();
    void CreateDepthStencilView();

private:
    bool InitializeShaders();
    void SetShaders(Shader* InVertexShader, Shader* InPixelShader) const;

    void CreateBuffer(const void* InBufferMemory, const D3D11_BUFFER_DESC* InBufferDesc, ID3D11Buffer** InBuffer) const;
    
private:
    void PreRender() const;
    void PostRender() const;
    
private:
    IDXGISwapChain* SwapChain;
    ID3D11Device* Device;
    ID3D11DeviceContext* DeviceContext;
    ID3D11RenderTargetView* RenderTargetView;
    ID3D11DepthStencilView* DepthStencilView;
    ID3D11Texture2D* DepthStencilBuffer;

    ID3D11Buffer* SquareIndexBuffer;
    ID3D11Buffer* SquareVertexBuffer;
    ID3D11InputLayout* VertexInputLayout;

    Shader* PixelShader;
    Shader* VertexShader;
    
    uint32 Width;
    uint32 Height;
};
