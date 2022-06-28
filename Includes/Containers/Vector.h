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

template<typename ElementType>
class Vector3 : public Vector2<ElementType>
{
public:
    Vector3() : Vector2(), Z(0) {}
    Vector3(ElementType InX, ElementType InY, ElementType InZ) : Vector2<ElementType>(InX, InY), Z(InZ) {}
    
public:
    ElementType Z;
};

template<typename ElementType>
class Vector4 : public Vector3<ElementType>
{
public:
    Vector4() : Vector3(), W(0) {}
    Vector4(ElementType InX, ElementType InY, ElementType InZ, ElementType InW) : Vector3<ElementType>(InX, InY, InZ), W(InW) {}
    
public:
    ElementType W;
};