#ifndef MODULE_CORE_LINE_SENSORS_HPP
#define MODULE_CORE_LINE_SENSORS_HPP

#ifdef WITHOUT_ARDUINO_ENVIRONMENT
#include "deps/arduino.hpp"
#else
#include <Arduino.h>
#endif

#include <array>
#include <utility>
#include "digital_reader.hpp"

namespace module
{
struct LineSensorsDirectionIndex
{
    const int8_t front;
    const int8_t left;
    const int8_t right;
    const int8_t back;
    const int8_t center;
    explicit constexpr LineSensorsDirectionIndex() : front(-1), left(-1), right(-1), back(-1), center(-1) {
    }
    explicit LineSensorsDirectionIndex(const int8_t _front, const int8_t _left, const int8_t _right, const int8_t _back, const int8_t _center)
        : front(_front), left(_left), right(_right), back(_back), center(_center) {
    }
};

class LineSensors
{
  public:
    explicit constexpr LineSensors(
        const pin_size_t interrupt_pin,
        const pin_size_t front_sensor_pin,
        const pin_size_t right_sensor_pin,
        const pin_size_t back_sensor_pin,
        const pin_size_t left_sensor_pin,
        const pin_size_t center_sensor_pin)
        : INTERRUPT_PIN_(interrupt_pin),
          Sensors_{
              module::DigitalReader(front_sensor_pin, INPUT_PULLUP),
              module::DigitalReader(right_sensor_pin, INPUT_PULLUP),
              module::DigitalReader(back_sensor_pin, INPUT_PULLUP),
              module::DigitalReader(left_sensor_pin, INPUT_PULLUP),
              module::DigitalReader(center_sensor_pin, INPUT_PULLUP),
          } {}

    void setup(void) const {
        pinMode(INTERRUPT_PIN_, INPUT_PULLUP);
        Sensors_[0].setup();
        Sensors_[1].setup();
        Sensors_[2].setup();
        Sensors_[3].setup();
        Sensors_[4].setup();
    }

    pin_size_t getInterruptPin(void) const {
        return digitalPinToInterrupt(INTERRUPT_PIN_);
    }

    bool isLineDetected(void) const {
        return digitalRead(INTERRUPT_PIN_) == HIGH;
    }

    std::pair<float, LineSensorsDirectionIndex> calculateLineAzimut(void) const {
        float az = 0;
        int8_t front = -1;
        if (Sensors_[0].isHigh()) {
            az = PI / 2;
            front = 0;
        } else if (Sensors_[1].isHigh()) {
            az = 0;
            front = 1;
        } else if (Sensors_[2].isHigh()) {
            az = -1 * PI / 2;
            front = 2;
        } else if (Sensors_[3].isHigh()) {
            az = PI;
            front = 3;
        }
        if (front == -1) {
            return {404, LineSensorsDirectionIndex()};
        }
        const int8_t left = (front + 3) % 4;
        const int8_t right = (front + 1) % 4;
        if (Sensors_[left].isHigh()) {
            az += -3 * PI / 4;
        } else if (Sensors_[right].isHigh()) {
            az += 3 * PI / 4;
        } else {
            az += PI;
        }
        return {az, LineSensorsDirectionIndex(front, left, right, (front + 2) % 4, 4)};
    }

    const module::DigitalReader& operator[](const int8_t index) const {
        if ((unsigned int)index >= 5) {
            Serial.println("Index out of range at LineSensors[index]");
            Serial.print("  index must lower than 5 but passed ");
            Serial.println(index);
            while (1) {}
        }
        return Sensors_[index];
    }

  private:
    const pin_size_t INTERRUPT_PIN_;
    const module::DigitalReader Sensors_[5];
};
}  // namespace module

#endif
