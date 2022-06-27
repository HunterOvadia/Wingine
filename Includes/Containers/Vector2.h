#pragma once

template<typename ElementType>
class Vector2
{
public:
    Vector2() : X(0), Y(0) {}
    Vector2(ElementType InX, ElementType InY) : X(InX), Y(InY) {}
    
public:
    ElementType X;
    ElementType Y;
};
