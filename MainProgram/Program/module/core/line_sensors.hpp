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
    const int front;
    const int left;
    const int right;
    const int back;
    const int center;
    explicit constexpr LineSensorsDirectionIndex() : front(-1), left(-1), right(-1), back(-1), center(-1) {
    }
    explicit LineSensorsDirectionIndex(const int _front, const int _left, const int _right, const int _back, const int _center)
        : front(_front), left(_left), right(_right), back(_back), center(_center) {
    }
};

class LineSensors
{
  public:
    explicit constexpr LineSensors(
        const int interrupt_pin,
        const int front_sensor_pin,
        const int right_sensor_pin,
        const int back_sensor_pin,
        const int left_sensor_pin,
        const int center_sensor_pin)
        : INTERRUPT_PIN_(interrupt_pin),
          Sensors_{
              module::DigitalReader(front_sensor_pin, INPUT_PULLUP),
              module::DigitalReader(right_sensor_pin, INPUT_PULLUP),
              module::DigitalReader(back_sensor_pin, INPUT_PULLUP),
              module::DigitalReader(left_sensor_pin, INPUT_PULLUP),
              module::DigitalReader(center_sensor_pin, INPUT_PULLUP),
          } {}

    void Setup() const {
        pinMode(INTERRUPT_PIN_, INPUT_PULLUP);
        for (int i = 0; i < 5; ++i) {
            Sensors_[i].Setup();
        }
    }

    int GetInterruptPin() const {
        return digitalPinToInterrupt(INTERRUPT_PIN_);
    }

    bool IsLineDetected() const {
        // bool ret = false;
        // for (int _ = 0; _ < 10; ++_) {
        //     ret |= (digitalRead(INTERRUPT_PIN_) == HIGH);
        //     delay(1);
        // }
        // return ret;
        return digitalRead(INTERRUPT_PIN_) == HIGH;
    }

    std::pair<float, LineSensorsDirectionIndex> CalculateLineAzimut() const {
        float az = 0;
        int front = -1;
        if (Sensors_[0].IsHigh()) {
            az = PI / 2;
            front = 0;
        } else if (Sensors_[1].IsHigh()) {
            az = 0;
            front = 1;
        } else if (Sensors_[2].IsHigh()) {
            az = -1 * PI / 2;
            front = 2;
        } else if (Sensors_[3].IsHigh()) {
            az = PI;
            front = 3;
        }
        if (front == -1) {
            return {404, LineSensorsDirectionIndex()};
        }
        const int left = (front + 3) % 4;
        const int right = (front + 1) % 4;
        if (Sensors_[left].IsHigh()) {
            az += -3 * PI / 4;
        } else if (Sensors_[right].IsHigh()) {
            az += 3 * PI / 4;
        } else {
            az += PI;
        }
        return {az, LineSensorsDirectionIndex(front, left, right, (front + 2) % 4, 4)};
    }

    const module::DigitalReader& operator[](const int index) const {
        if ((unsigned int)index >= 5) {
            Serial.println("Index out of range at LineSensors[index]");
            Serial.print("  index must lower than 5 but passed ");
            Serial.println(index);
            while (1) {}
        }
        return Sensors_[index];
    }

  private:
    const int INTERRUPT_PIN_;
    const module::DigitalReader Sensors_[5];
};
}  // namespace module

#endif
