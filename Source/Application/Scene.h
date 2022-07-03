#pragma once
#include <DirectXMath.h>

class Scene
{
public:
    Scene() = default;
    virtual ~Scene() = default;

    virtual void Initialize() = 0;
    virtual void Update() = 0;
    virtual void Render() = 0;
    virtual void Cleanup() = 0;
};

class TestScene : public Scene
{
public:
    TestScene(): Cube1World(), Cube2World()
    {
    }

    void Initialize() override
    {
        
    }

    void Update() override
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

    void Render() override
    {

    }

    void Cleanup() override
    {
        
    }

private:
    DirectX::XMMATRIX Cube1World;
    DirectX::XMMATRIX Cube2World;
    float Rot = 0.01f;
};