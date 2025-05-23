#include "Vector2D.hpp"
#include <iostream>

Vector2D& Vector2D::operator=(const Vector2D &other)
{
    if (this != &other) 
    {  // Prevent self-assignment
        x = other.x;
        y = other.y;
    }
    return *this;
}

const float& Vector2D::operator[](int index) const
{
    if (index == 0) return x;
    else if (index == 1) return y;
    throw std::out_of_range("Index out of range");
}

Vector2D& Vector2D::operator+=(const Vector2D &addend)
{
    x += addend.x; 
    y += addend.y; 
    return *this; 
}

float& Vector2D::operator[](int index)
{
    if (index == 0) return x;
    else if (index == 1) return y;
    else throw std::out_of_range("Index out of range");
    
}



