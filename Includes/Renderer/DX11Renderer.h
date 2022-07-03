#pragma once
#include <d3d11.h>
#include <dxgi.h>
#include <DirectXMath.h>
#include "Texture.h"
#include "Core/Application.h"
#include "Renderer.h"

#define HR_CHECK(HR) if(FAILED(HR)) { WIN_LOG(Error, "DirectX Creation Failure: %s - %d", __FILE__, __LINE__); }
#define DX_SAFE_RELEASE(Com) if(Com) { (Com)->Release(); (Com) = nullptr; }

class Shader;


struct CameraData
{
    DirectX::XMMATRIX View;
    DirectX::XMMATRIX Projection;
    DirectX::XMVECTOR Position;
    DirectX::XMVECTOR Target;
    DirectX::XMVECTOR Up;

    void Initialize(const uint32 Width, const uint32 Height);
};

struct ConstantBufferPerObjectData
{
    DirectX::XMMATRIX WVP;
    void UpdateData(const DirectX::XMMATRIX& InWorld, const CameraData& InCamera)
    {
        WVP = XMMatrixTranspose(InWorld * InCamera.View * InCamera.Projection);
    }
};


class DX11Renderer : public IRenderer
{
public:
    DX11Renderer();

public:
    ID3D11Device* GetDevice() const { return Device; }
    
public:
    bool Initialize(Window* MainWindow) override;
    void Shutdown() override;

    // TODO(HO): Scenes
    bool InitializeScene() override;
    void UpdateScene(float64 DeltaTime) override;
    void RenderScene() override;   
    void CleanupScene() override;
    
    void PreRender() override;
    void PostRender() override;
    void CreateTexture(void* InTextureData, uint32 InWidth, uint32 InHeight, void** OutData) override;
    void DestroyTexture(Texture* InTexture) override;
    
    void CreateShader(ShaderType InType, void* InShaderData, uint64 InSize, void** OutShader) override;
    void DestroyShader(Shader* InShader) override;
    void ResizeViewport(float Width, float Height) override;
    
private:
    void SetShader(const Shader* InShader) const;
    void SetConstantBufferData(const DirectX::XMMATRIX& InWVP);
    void CreateDeviceAndSwapChain(Window* MainWindow);
    void CreateRenderTargetView();
    void CreateDepthStencilView();
    void CreateBuffer(const void* InBufferMemory, const D3D11_BUFFER_DESC* InBufferDesc, ID3D11Buffer** InBuffer) const;
    void CreateSamplerState();
    void CreateRasterizerStates();
    void CreateBlendState();
    void SetDefaultBlendState();
    
    void CreateIndexBuffer();
    void CreateVertexBuffer();
    void CreateConstantBuffer();
    void CreateInputLayout();
    void ClearViews();
    void SetTexture(ID3D11ShaderResourceView* ResourceView, ID3D11SamplerState* SamplerState);
    
private:
    bool InitializeShaders();
    bool InitializeTextures();

private:
    IDXGISwapChain* SwapChain;
    ID3D11Device* Device;
    ID3D11DeviceContext* DeviceContext;
    ID3D11RenderTargetView* RenderTargetView;
    ID3D11DepthStencilView* DepthStencilView;
    ID3D11Texture2D* DepthStencilBuffer;
    ID3D11Buffer* ConstantBuffer;
    
    ID3D11RasterizerState* WireFrameRasterizerState;
    ID3D11RasterizerState* CCWRasterizerState;
    ID3D11RasterizerState* CWRasterizerState;
    ID3D11RasterizerState* NoCullRasterizerState;
    
    ID3D11BlendState* BlendState;

    ID3D11Buffer* SquareIndexBuffer;
    ID3D11Buffer* SquareVertexBuffer;
    ID3D11InputLayout* VertexInputLayout;

    Shader* PixelShader;
    Shader* VertexShader;
    Texture* CubeTexture;

    ID3D11SamplerState* SampleState;
    
    uint32 Width;
    uint32 Height;
    
    // TOOD(HO): Not the right place for this
    CameraData Camera;
    ConstantBufferPerObjectData ConstantBufferData;
    DirectX::XMMATRIX Cube1World;
    DirectX::XMMATRIX Cube2World;
    float32 Rot = 0.01f;

};


