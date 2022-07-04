#pragma once
#include <d3d11.h>
#include <dxgi.h>
#include <DirectXMath.h>
#include "Light.h"
#include "Texture.h"
#include "Core/Application.h"
#include "Renderer.h"

#define HR_CHECK(HR) if(FAILED(HR)) { WIN_LOG(Error, "DirectX Creation Failure: %s - %d", __FILE__, __LINE__); }
#define DX_SAFE_RELEASE(Com) if(Com) { (Com)->Release(); (Com) = nullptr; }

class Shader;

struct CameraData
{
public:
    void Update(float64 Time);

public:
    DirectX::XMMATRIX View;
    DirectX::XMMATRIX Projection;
    DirectX::XMVECTOR Position;
    DirectX::XMVECTOR Target;
    DirectX::XMVECTOR Up;


    DirectX::XMVECTOR CamForward = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
    DirectX::XMVECTOR CamRight = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

    DirectX::XMMATRIX CamRotationMatrix;
    
    float MoveLeftRight = 0.0f;
    float MoveBackForward = 0.0;
    float CamYaw = 0.0f;
    float CamPitch = 0.0f;

public:
    inline static DirectX::XMVECTOR DefaultForward = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
    inline static DirectX::XMVECTOR DefaultRight = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
};

struct CBPerObject
{
    DirectX::XMMATRIX WVP;
    DirectX::XMMATRIX World;
    void UpdateData(const DirectX::XMMATRIX& InWorld, const CameraData& InCamera)
    {
        const DirectX::XMMATRIX PreTransposeWVP = InWorld * InCamera.View * InCamera.Projection;
        WVP = XMMatrixTranspose(PreTransposeWVP);
        World = XMMatrixTranspose(InWorld);
    }
};

struct CBPerFrame
{
    Light WorldLight;
    void UpdateData(const Light& InLight)
    {
        WorldLight = InLight;
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
    void UpdatePerObjectConstantBuffer(const DirectX::XMMATRIX& InWorld);
    void UpdatePerFrameConstantBuffer();
    void CreateDeviceAndSwapChain(Window* MainWindow);
    void CreateRenderTargetView();
    void CreateDepthStencilView();
    void CreateBuffer(const void* InBufferMemory, const D3D11_BUFFER_DESC* InBufferDesc, ID3D11Buffer** InBuffer) const;
    void CreateSamplerState();
    void CreateBlendState();
    void SetDefaultBlendState();
    
    void CreateIndexBuffer();
    void CreateVertexBuffer();
    void CreateConstantBuffers();
    void CreateInputLayout();
    void ClearViews();
    void SetTexture(ID3D11ShaderResourceView* ResourceView, ID3D11SamplerState* SamplerState);

    void SetIndexBuffer(ID3D11Buffer* InBuffer);
    void SetVertexBuffer(ID3D11Buffer* InBuffer);
    
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
    ID3D11BlendState* BlendState;
    ID3D11SamplerState* SamplerState;

    ID3D11Buffer* PerObjectConstantBuffer;
    ID3D11Buffer* PerFrameConstantBuffer;
    
    ID3D11Buffer* SquareIndexBuffer;
    ID3D11Buffer* SquareVertexBuffer;
    ID3D11InputLayout* VertexInputLayout;

    Shader* PixelShader;
    Shader* VertexShader;
    Texture* CubeTexture;
    
    uint32 Width;
    uint32 Height;
    
    CBPerObject ConstantBufferPerObject;
    CBPerFrame ConstantBufferPerFrame;

    // TOOD(HO): Not the right place for this
    Light WorldLight;
    CameraData Camera;
    DirectX::XMMATRIX GroundWorld;

};


