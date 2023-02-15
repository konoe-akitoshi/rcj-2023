#ifndef VECTOR2_HPP
#define VECTOR2_HPP

#include <math.h>

struct Vector2
{
    float x;
    float y;

    constexpr Vector2() : x(0), y(0) {}
    constexpr Vector2(const float _x, const float _y) : x(_x), y(_y) {}
    constexpr Vector2(const int _x, const int _y) : x(_x), y(_y) {}

    constexpr Vector2 operator+() const {
        return *this;
    }

    constexpr Vector2 operator-() const {
        return {-x, -y};
    }

    constexpr Vector2 operator+(const Vector2& other) const {
        return {x + other.x, y + other.y};
    }

    constexpr Vector2 operator-(const Vector2& other) const {
        return {x - other.x, y - other.y};
    }

    constexpr Vector2 operator*(const float s) const {
        return {x * s, y * s};
    }

    constexpr Vector2 operator/(const float s) const {
        return {x / s, y / s};
    }

    inline Vector2& operator+=(const Vector2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    inline Vector2& operator-=(const Vector2& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    inline Vector2& operator*=(const float s) {
        x *= s;
        y *= s;
        return *this;
    }

    inline Vector2& operator/=(const float s) {
        x /= s;
        y /= s;
        return *this;
    }

    static constexpr float SquareNorm(const Vector2& vec) {
        return vec.x * vec.x + vec.y * vec.y;
    }

    static constexpr float Norm(const Vector2& vec) {
        return sqrt(vec.x * vec.x + vec.y * vec.y);
    }

    static constexpr float Angle(const Vector2& vec) {
        return atan2(100 * vec.y, 100 * vec.x);
    }
};

#endif
