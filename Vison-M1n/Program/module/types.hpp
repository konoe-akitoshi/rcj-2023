#ifndef MODULE_TYPES_HPP
#define MODULE_TYPES_HPP

struct Vector2Int
{
    const int x;
    const int y;
    constexpr Vector2Int(const int _x, const int _y) : x(_x), y(_y) {}
};

struct CameraFieldData
{
    const Vector2Int ballPosition;
    const Vector2Int yellowGoalPosition;
    const Vector2Int blueGoalPosition;
    const int yellowGoalWidth;
    const int blueGoalWidth;

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
};

#endif
