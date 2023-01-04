#ifndef COMPONENTS_GYRO_HPP
#define COMPONENTS_GYRO_HPP

#ifdef LOCAL_INCLUDE
#include "../../local/arduino_deps.h"
#endif

#include "setup_handler.hpp"

namespace component
{
class Gyro
{
  public:
    explicit Gyro(SetupHandler& handler, const int speed);
    int GetRotation() const;
};

inline Gyro::Gyro(SetupHandler& handler, const int speed) {
    handler.SetCallback([speed]() {
        Serial2.begin(speed);
    });
}

inline int Gyro::GetRotation() const {
    static int prev = 255;
    if (Serial2.available() == 0) {
        return prev;
    }
    int ret = 0;
    while (Serial2.available() != 0) {
        // Serial2の送信側がint8_tで送ってるので、intで受け取ると負の数が期待通り受け取れない。
        // そのため、int8_tにキャストする必要がある。
        const int8_t raw = Serial2.read();
        const int val = (int)raw;
        ret = val * 100 / 128;
    }
    prev = ret;
    return ret;
}

}  // namespace component

#endif
