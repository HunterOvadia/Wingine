#pragma once
#include <d3d11.h>

#include "Containers/Vector.h"

class Vertex
{
public:
    Vertex() : Vertex({0,0,0}, {0.0f, 0.0f, 0.0f, 1.0f}) {}
    Vertex(const Vector3<float>& InPosition, const Vector4<float>& InColor) : Position(InPosition), Color(InColor) {}
    
public:
    Vector3<float> Position;
    Vector4<float> Color;

public:
    inline static D3D11_INPUT_ELEMENT_DESC Layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    inline static UINT LayoutCount = ARRAYSIZE(Layout);
};
