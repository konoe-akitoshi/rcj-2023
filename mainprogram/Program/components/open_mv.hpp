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
    void WaitData();
    int GetBallCount() const;
    int GetYellowGoalCount() const;
    int GetBlueGoalCount() const;
    Vector2 GetBallPosition() const;
    Vector2 GetYellowGoalPosition() const;
    Vector2 GetBlueGoalPosition() const;

    // ↓なんのためにある？
    inline int BlobCount() const {
        return raw_[0];
    }

  private:
    std::array<int, 39> raw_;
    inline int GetDataUnit_() const {
        while (Serial3.available() == 0) {
            ;  // wait for serial data
        }
        return Serial3.read();
    }
};

inline OpenMV::OpenMV(const int speed) {
    Serial3.begin(speed);
}

inline void OpenMV::WaitData() {
    while (true) {
        raw_[0] = GetDataUnit_();
        if (raw_[0] == 254) {
            break;
        }
    }
    for (int i = 1; i < 39; ++i) {
        raw_[i] = GetDataUnit_();
    }
}

inline int OpenMV::GetBallCount() const {
    return raw_[1];
}

inline int OpenMV::GetYellowGoalCount() const {
    return raw_[14];
}

inline int OpenMV::GetBlueGoalCount() const {
    return raw_[27];
}

inline Vector2 OpenMV::GetBallPosition() const {
    const int x_last6 = raw_[5] & 0b111111;
    const int x_upper6 = raw_[6] & 0b111111;
    const int y_last6 = raw_[7] & 0b111111;
    const int y_upper6 = raw_[8] & 0b111111;
    return {x_last6 + (x_upper6 << 6), y_last6 + (y_upper6 << 6)};
}

inline Vector2 OpenMV::GetYellowGoalPosition() const {
    const int x_last6 = raw_[18] & 0b111111;
    const int x_upper6 = raw_[19] & 0b111111;
    const int y_last6 = raw_[20] & 0b111111;
    const int y_upper6 = raw_[21] & 0b111111;
    return {x_last6 + (x_upper6 << 6), y_last6 + (y_upper6 << 6)};
}

inline Vector2 OpenMV::GetBlueGoalPosition() const {
    const int x_last6 = raw_[31] & 0b111111;
    const int x_upper6 = raw_[32] & 0b111111;
    const int y_last6 = raw_[33] & 0b111111;
    const int y_upper6 = raw_[34] & 0b111111;
    return {x_last6 + (x_upper6 << 6), y_last6 + (y_upper6 << 6)};
}

}  // namespace component

#endif
