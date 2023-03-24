#ifndef MODULE_TYPES_HPP
#define MODULE_TYPES_HPP

#ifdef WITHOUT_ARDUINO_ENVIRONMENT
#include "core/deps/arduino.hpp"
#else
#include "Arduino.h"
#endif

struct Vector2Int
{
    int x;
    int y;
    constexpr Vector2Int(const int _x, const int _y) : x(_x), y(_y) {}

    Vector2Int(const Vector2Int&) = default;
    Vector2Int(Vector2Int&&) = default;
    Vector2Int& operator=(const Vector2Int&) = default;
    Vector2Int& operator=(Vector2Int&&) = default;

    void operator=(const Vector2Int& other) volatile {
        x = other.x;
        y = other.y;
    }

    constexpr Vector2Int operator+(const Vector2Int& other) const {
        return {x + other.x, y + other.y};
    }

    constexpr Vector2Int operator-(const Vector2Int& other) const {
        return {x - other.x, y - other.y};
    }

    constexpr Vector2Int operator*(const float s) const {
        return {(int)(x * s), (int)(y * s)};
    }

    constexpr Vector2Int operator/(const float s) const {
        return {(int)(x / s), (int)(y / s)};
    }

    inline Vector2Int& operator+=(const Vector2Int& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    inline Vector2Int& operator-=(const Vector2Int& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    inline Vector2Int& operator*=(const float s) {
        x *= s;
        y *= s;
        return *this;
    }

    inline Vector2Int& operator/=(const float s) {
        x /= s;
        y /= s;
        return *this;
    }
};

struct CameraFieldData
{
    Vector2Int ballPosition;
    Vector2Int yellowGoalPosition;
    Vector2Int blueGoalPosition;
    bool ballExist;
    bool yellowGoalExist;
    bool blueGoalExist;
    int yellowGoalWidth;
    int blueGoalWidth;

    explicit constexpr CameraFieldData()
        : ballPosition(-1, -1),
          yellowGoalPosition(-1, -1),
          blueGoalPosition(-1, -1),
          ballExist(false),
          yellowGoalExist(false),
          blueGoalExist(false),
          yellowGoalWidth(-1),
          blueGoalWidth(-1) {}

    explicit constexpr CameraFieldData(
        const Vector2Int _ballPosition,
        const Vector2Int _yellowGoalPosition,
        const Vector2Int _blueGoalPosition,
        bool _ballExist,
        bool _yellowGoalExist,
        bool _blueGoalExist,
        const int _yellowGoalWidth,
        const int _blueGoalWidth)
        : ballPosition(_ballPosition),
          yellowGoalPosition(_yellowGoalPosition),
          blueGoalPosition(_blueGoalPosition),
          ballExist(_ballExist),
          yellowGoalExist(_yellowGoalExist),
          blueGoalExist(_blueGoalExist),
          yellowGoalWidth(_yellowGoalWidth),
          blueGoalWidth(_blueGoalWidth) {}

    static void dumpToSerial(volatile const CameraFieldData& data) {
        Serial.println("{");

        Serial.print("  ballPosition: (");
        Serial.print(data.ballPosition.x);
        Serial.print(", ");
        Serial.print(data.ballPosition.y);
        Serial.println(")");

        Serial.print("  yellowGoalPosition: (");
        Serial.print(data.yellowGoalPosition.x);
        Serial.print(", ");
        Serial.print(data.yellowGoalPosition.y);
        Serial.println(")");

        Serial.print("  blueGoalPosition: (");
        Serial.print(data.blueGoalPosition.x);
        Serial.print(", ");
        Serial.print(data.blueGoalPosition.y);
        Serial.println(")");

        Serial.print("  ballExist: ");
        Serial.println(data.ballExist ? "true" : "false");

        Serial.print("  yellowGoalExist: ");
        Serial.println(data.yellowGoalExist ? "true" : "false");

        Serial.print("  blueGoalExist: ");
        Serial.println(data.blueGoalExist ? "true" : "false");

        Serial.print("  yellowGoalWidth: ");
        Serial.println(data.yellowGoalWidth);

        Serial.print("  blueGoalWidth: ");
        Serial.println(data.blueGoalWidth);

        Serial.println("}");
    }
};

#endif
