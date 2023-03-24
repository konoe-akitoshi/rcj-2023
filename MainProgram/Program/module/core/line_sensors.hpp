#ifndef MODULE_CORE_LINE_SENSORS_HPP
#define MODULE_CORE_LINE_SENSORS_HPP

#ifdef WITHOUT_ARDUINO_ENVIRONMENT
#include "deps/arduino.hpp"
#else
#include <Arduino.h>
#endif

#include "digital_reader.hpp"

namespace module
{
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

    float calculateLineAzimut(void) const {
        float az = 0;
        if (Sensors_[4].isHigh()) {
            return 100;
        }
        bool front = Sensors_[0].isHigh();
        bool right = Sensors_[1].isHigh();
        bool back = Sensors_[2].isHigh();
        bool left = Sensors_[3].isHigh();
        if (front && right) {
            az = 1 * PI / 4;
        } else if (front && left) {
            az = 3 * PI / 4;
        } else if (back && right) {
            az = -1 * PI / 4;
        } else if (back && left) {
            az = -3 * PI / 4;
        } else if (front) {
            az = 1 * PI / 2;
        } else if (right) {
            az = 0 * PI / 2;
        } else if (left) {
            az = 2 * PI / 2;
        } else if (back) {
            az = -1 * PI / 2;
        }
        return az;
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
