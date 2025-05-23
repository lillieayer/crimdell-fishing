#ifndef VECTOR2D_HPP
#define VECTOR2D_HPP

#include <cmath>

class Vector2D {
    private: 
        float x, y;

    public: 
        Vector2D(float x, float y) : x(x), y(y) {}

        Vector2D operator+(const Vector2D& addend) const { return Vector2D(x + addend.x, y + addend.y); }
        Vector2D operator*(const Vector2D& factor) const { return Vector2D(x * factor.x, y * factor.y); }
        Vector2D& operator+=(const Vector2D& addend);
        Vector2D& operator=(const Vector2D& other);
        const float& operator[](int index) const; // allows read access
        float& operator[](int index); // allows write access


};
#endif // VECTOR2D_HPP