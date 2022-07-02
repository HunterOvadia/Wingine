#pragma once
#include <d3d11.h>

#include "Containers/Vector.h"
#include "Defines.h"

class Vertex
{
public:
    Vertex() : Vertex({0,0,0}, {0.0f, 0.0f}) {}
    Vertex(const Vector3<float>& InPosition, const Vector2<float>& InTexCoord) : Position(InPosition), TexCoord(InTexCoord) {}
    
public:
    Vector3<float> Position;
    Vector2<float> TexCoord;

public:
    inline static D3D11_INPUT_ELEMENT_DESC Layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    inline static UINT LayoutCount = STATIC_ARRAY_SIZE(Layout);
};
