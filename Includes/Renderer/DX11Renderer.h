#pragma once
#include <d3d11.h>
#include <dxgi.h>

#include "Core/Application.h"

#define HR_CHECK(HR) if(FAILED(HR)) { WIN_LOG(Error, "DirectX Creation Failure: %s - %d", __FILE__, __LINE__); }
#define DX_SAFE_RELEASE(Com) if(Com) { Com->Release(); Com = nullptr; }

class Shader;
class DX11Renderer
{
public:
    DX11Renderer();
    bool Initialize(Window* MainWindow);
    void Shutdown();
    void Render();
    bool SetupScene();
    void SetShader(const Shader* InShader) const;
    
private:
    void PreRender();
    void PostRender();
    
private:
    IDXGISwapChain* SwapChain;
    ID3D11Device* Device;
    ID3D11DeviceContext* DeviceContext;
    ID3D11RenderTargetView* RenderTargetView;
    
    ID3D11Buffer* SquareIndexBuffer;
    ID3D11Buffer* SquareVertexBuffer;
    ID3D11InputLayout* VertexInputLayout;

    Shader* PixelShader;
    Shader* VertexShader;
    
    uint32 Width;
    uint32 Height;
};
