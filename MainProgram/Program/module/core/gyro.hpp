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
    explicit constexpr Gyro(const int speed, const pin_size_t reset_input_pin, pin_size_t reset_output_pin)
        : SPEED_(speed), RESET_INPUT_PIN_(reset_input_pin), RESET_OUTPUT_PIN_(reset_output_pin) {
    }

    void setup(void) const {
        do {
            Serial2.begin(SPEED_);
            delay(100);
        } while (!Serial2);
        pinMode(RESET_INPUT_PIN_, INPUT_PULLUP);
        pinMode(RESET_OUTPUT_PIN_, OUTPUT);
    }

    int8_t getRotation(void) const {
        static int8_t prev = 0;

        auto reset_status = digitalRead(RESET_INPUT_PIN_);
        digitalWrite(RESET_OUTPUT_PIN_, reset_status);

        if (Serial2.available() == 0) {
            return prev;
        }
        int8_t ret = 0;
        while (Serial2.available() != 0) {
            // Serial2の送信側がint8_tで送ってるので、intで受け取ると負の数が期待通り受け取れない。
            // そのため、int8_tにキャストする必要がある。
            const int8_t raw = Serial2.read();
            const int16_t val = (int16_t)raw;
            ret = val * 100 / 128;
        }
        prev = ret;
        return ret;
    }

  private:
    const int SPEED_;
    const pin_size_t RESET_INPUT_PIN_;
    const pin_size_t RESET_OUTPUT_PIN_;
};
}  // namespace module

#endif
