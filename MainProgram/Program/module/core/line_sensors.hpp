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
    int8_t front;
    int8_t right;
    int8_t back;
    int8_t left;
    int8_t center;
    explicit constexpr LineSensorsDirectionIndex() : front(-1), right(-1), back(-1), left(-1), center(-1) {
    }
    explicit constexpr LineSensorsDirectionIndex(const int8_t _front, const int8_t _right, const int8_t _back, const int8_t _left, const int8_t _center)
        : front(_front), right(_right), back(_back), left(_left), center(_center) {
    }
    LineSensorsDirectionIndex(const volatile LineSensorsDirectionIndex& other)
        : front(other.front), right(other.right), back(other.right), left(other.left), center(other.center) {
    }

    LineSensorsDirectionIndex(const LineSensorsDirectionIndex&) = default;
    LineSensorsDirectionIndex(LineSensorsDirectionIndex&&) = default;
    LineSensorsDirectionIndex& operator=(const LineSensorsDirectionIndex&) = default;
    LineSensorsDirectionIndex& operator=(LineSensorsDirectionIndex&&) = default;

    void operator=(const LineSensorsDirectionIndex& other) volatile {
        front = other.front;
        left = other.left;
        right = other.right;
        back = other.back;
        center = other.center;
    }
};

constexpr std::pair<float, LineSensorsDirectionIndex> NOT_FOUND_LINE_AZIMUT = {404, module::LineSensorsDirectionIndex()};

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
            return NOT_FOUND_LINE_AZIMUT;
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
        return {az, LineSensorsDirectionIndex(front, right, (front + 2) % 4, left, 4)};
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
