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
};

struct CameraFieldData
{
    Vector2Int ballPosition;
    Vector2Int yellowGoalPosition;
    Vector2Int blueGoalPosition;
    int yellowGoalWidth;
    int blueGoalWidth;

    explicit constexpr CameraFieldData()
        : ballPosition(-1, -1),
          yellowGoalPosition(-1, -1),
          blueGoalPosition(-1, -1),
          yellowGoalWidth(-1),
          blueGoalWidth(-1) {}

    explicit constexpr CameraFieldData(
        const Vector2Int _ballPosition,
        const Vector2Int _yellowGoalPosition,
        const Vector2Int _blueGoalPosition,
        const int _yellowGoalWidth,
        const int _blueGoalWidth)
        : ballPosition(_ballPosition),
          yellowGoalPosition(_yellowGoalPosition),
          blueGoalPosition(_blueGoalPosition),
          yellowGoalWidth(_yellowGoalWidth),
          blueGoalWidth(_blueGoalWidth) {}

    void dumpToSerial() const {
        Serial.println("{");

        Serial.print("  ballPosition: (");
        Serial.print(ballPosition.x);
        Serial.print(", ");
        Serial.print(ballPosition.y);
        Serial.println(")");

        Serial.print("  yellowGoalPosition: (");
        Serial.print(yellowGoalPosition.x);
        Serial.print(", ");
        Serial.print(yellowGoalPosition.y);
        Serial.println(")");

        Serial.print("  blueGoalPosition: (");
        Serial.print(blueGoalPosition.x);
        Serial.print(", ");
        Serial.print(blueGoalPosition.y);
        Serial.println(")");

        Serial.print("  yellowGoalWidth: ");
        Serial.println(yellowGoalWidth);

        Serial.print("  blueGoalWidth: ");
        Serial.println(blueGoalWidth);

        Serial.println("}");
    }
};

#endif
