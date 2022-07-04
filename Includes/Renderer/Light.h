#pragma once
#include "Defines.h"

struct Light
{
    DirectX::XMFLOAT3 Direction;
    float32 Padding1;

    DirectX::XMFLOAT3 Position;
    float32 Range;
    
    DirectX::XMFLOAT3 Attenuation;
    float32 Padding2;

    DirectX::XMFLOAT4 Ambient;
    DirectX::XMFLOAT4 Diffuse;
};
