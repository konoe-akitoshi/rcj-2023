#ifndef MODULE_CORE_GYRO_HPP
#define MODULE_CORE_GYRO_HPP

#ifdef WITHOUT_ARDUINO_ENVIRONMENT
#include "deps/arduino.hpp"
#else
#include <Arduino.h>
#endif

namespace module
{
class Gyro
{
  public:
    explicit constexpr Gyro(const int speed) : SPEED_(speed) {
    }

    void setup(void) const {
        do {
            Serial1.begin(SPEED_);
            delay(100);
        } while (Serial1 == false);
    }

    int getRotation(void) const {
        static int prev = 0;
        if (Serial1.available() == 0) {
            return prev;
        }
        int ret = 0;
        while (Serial1.available() != 0) {
            // Serial2の送信側がint8_tで送ってるので、intで受け取ると負の数が期待通り受け取れない。
            // そのため、int8_tにキャストする必要がある。
            const int8_t raw = Serial1.read();
            const int val = (int)raw;
            ret = val * 100 / 128;
        }
        prev = ret;
        return ret;
    }

  private:
    const int SPEED_;
};
}  // namespace module

#endif
