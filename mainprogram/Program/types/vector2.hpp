#ifndef TYPE_VECTOR2_HPP
#define TYPE_VECTOR2_HPP

#include <math.h>

struct Vector2
{
    float x;
    float y;
    constexpr Vector2();
    constexpr Vector2(const float _x, const float _y);
    constexpr Vector2(const int _x, const int _y);
    constexpr Vector2 operator+() const;
    constexpr Vector2 operator-() const;
    constexpr Vector2 operator+(const Vector2& other) const;
    constexpr Vector2 operator-(const Vector2& other) const;
    constexpr Vector2 operator*(const float s) const;
    constexpr Vector2 operator/(const float s) const;
    inline Vector2& operator+=(const Vector2& other);
    inline Vector2& operator-=(const Vector2& other);
    inline Vector2& operator*=(const float s);
    inline Vector2& operator/=(const float s);

    static constexpr float squareNorm(const Vector2& vec);
    static constexpr float norm(const Vector2& vec);
};

constexpr Vector2::Vector2() : x(0), y(0) {}
constexpr Vector2::Vector2(const float _x, const float _y) : x(_x), y(_y) {}
constexpr Vector2::Vector2(const int _x, const int _y) : x(_x), y(_y) {}
constexpr Vector2 Vector2::operator+() const {
    return *this;
}
constexpr Vector2 Vector2::operator-() const {
    return {-x, -y};
}
constexpr Vector2 Vector2::operator+(const Vector2& other) const {
    return {x + other.x, y + other.y};
}
constexpr Vector2 Vector2::operator-(const Vector2& other) const {
    return {x - other.x, y - other.y};
}
constexpr Vector2 Vector2::operator*(const float s) const {
    return {x * s, y * s};
}
constexpr Vector2 Vector2::operator/(const float s) const {
    return {x / s, y / s};
}
inline Vector2& Vector2::operator+=(const Vector2& other) {
    x += other.x;
    y += other.y;
    return *this;
}
inline Vector2& Vector2::operator-=(const Vector2& other) {
    x -= other.x;
    y -= other.y;
    return *this;
}
inline Vector2& Vector2::operator*=(const float s) {
    x *= s;
    y *= s;
    return *this;
}
inline Vector2& Vector2::operator/=(const float s) {
    x /= s;
    y /= s;
    return *this;
}

constexpr float Vector2::squareNorm(const Vector2 &vec) {
    return vec.x * vec.x + vec.y * vec.y;
}

constexpr float Vector2::norm(const Vector2 &vec) {
    return sqrt(vec.x * vec.x + vec.y * vec.y);
}

#endif
