#ifndef COMPONENT_OPEN_VM_HPP
#define COMPONENT_OPEN_VM_HPP

#ifdef LOCAL_INCLUDE
#include "../../local/arduino_deps.h"
#endif

#include <array>
#include "../types/vector2.hpp"

namespace component
{

class OpenMV
{
  public:
    explicit OpenMV(const int speed);
    void wait_data();
    int get_ball_count() const;
    int get_yellow_goal_count() const;
    int get_blue_goal_count() const;
    Vector2 get_ball_position() const;
    Vector2 get_yellow_goal_position() const;
    Vector2 get_blue_goal_position() const;

    // ↓なんのためにある？
    inline int blob_count() const {
        return raw[0];
    }

  private:
    std::array<int, 39> raw;
    inline int get_data_unit() const {
        while (Serial3.available() == 0) {
            ;  // wait for serial data
        }
        return Serial3.read();
    }
};

inline OpenMV::OpenMV(const int speed) {
    Serial3.begin(speed);
}

inline void OpenMV::wait_data() {
    while (true) {
        raw[0] = get_data_unit();
        if (raw[0] == 254) {
            break;
        }
    }
    for (int i = 1; i < 39; ++i) {
        raw[i] = get_data_unit();
    }
}

inline int OpenMV::get_ball_count() const {
    return raw[1];
}

inline int OpenMV::get_yellow_goal_count() const {
    return raw[14];
}

inline int OpenMV::get_blue_goal_count() const {
    return raw[27];
}

inline Vector2 OpenMV::get_ball_position() const {
    const int x_last6 = raw[5] & 0b111111;
    const int x_upper6 = raw[6] & 0b111111;
    const int y_last6 = raw[7] & 0b111111;
    const int y_upper6 = raw[8] & 0b111111;
    return {x_last6 + (x_upper6 << 6), y_last6 + (y_upper6 << 6)};
}

inline Vector2 OpenMV::get_yellow_goal_position() const {
    const int x_last6 = raw[18] & 0b111111;
    const int x_upper6 = raw[19] & 0b111111;
    const int y_last6 = raw[20] & 0b111111;
    const int y_upper6 = raw[21] & 0b111111;
    return {x_last6 + (x_upper6 << 6), y_last6 + (y_upper6 << 6)};
}

inline Vector2 OpenMV::get_blue_goal_position() const {
    const int x_last6 = raw[31] & 0b111111;
    const int x_upper6 = raw[32] & 0b111111;
    const int y_last6 = raw[33] & 0b111111;
    const int y_upper6 = raw[34] & 0b111111;
    return {x_last6 + (x_upper6 << 6), y_last6 + (y_upper6 << 6)};
}

}  // namespace component

#endif
