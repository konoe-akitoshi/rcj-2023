#ifndef MODULE_CORE_LED_HPP
#define MODULE_CORE_LED_HPP

#include "i2c_manager.hpp"

#ifdef WITHOUT_ARDUINO_ENVIRONMENT
#include "deps/arduino.hpp"
#else
#include <Arduino.h>
#endif

namespace module
{
class LedLight
{
  public:
    /**
     * @param pin pin number.
     * @param on  the value(HIGH or LOW) to Write to the specified pin if you want to tern on the LED.
     * @param off  the value(HIGH or LOW) to Write to the specified pin if you want to tern off the LED.
     */
    explicit constexpr LedLight(const pin_size_t pin, const int on = HIGH, const int off = LOW)
        : PIN_(pin), MODE_ON_(on), MODE_OFF_(off) {
    }

    void setup(void) const {
        pinMode(PIN_, OUTPUT);
        digitalWrite(PIN_, MODE_OFF_);
    }

    /**
     * Tern on the LED light.
     */
    void ternOn() const {
        digitalWrite(PIN_, MODE_ON_);
    }

    /**
     * Tern off the LED light.
     */
    void ternOff() const {
        digitalWrite(PIN_, MODE_OFF_);
    }

  private:
    const pin_size_t PIN_;
    const int MODE_ON_;
    const int MODE_OFF_;
};

class LedLightPCF8574
{
  public:
    /**
     * @param pin pin number.
     * @param on  the value(HIGH or LOW) to Write to the specified pin if you want to tern on the LED.
     * @param off  the value(HIGH or LOW) to Write to the specified pin if you want to tern off the LED.
     */
    explicit constexpr LedLightPCF8574(const I2CManager& i2c_manager, const uint8_t pin, const int on = HIGH, const int off = LOW)
        : I2C_MANAGER_(i2c_manager), PIN_(pin), MODE_ON_(on), MODE_OFF_(off) {
    }

    void setup(void) const {
        I2C_MANAGER_.beginPCF8574();
        I2C_MANAGER_.pcf8574_pinMode(PIN_, OUTPUT);
        I2C_MANAGER_.pcf8574_digitalWrite(PIN_, MODE_OFF_);
    }

    /**
     * Tern on the LED light.
     */
    void ternOn() const {
        I2C_MANAGER_.pcf8574_digitalWrite(PIN_, MODE_ON_);
    }

    /**
     * Tern off the LED light.
     */
    void ternOff() const {
        I2C_MANAGER_.pcf8574_digitalWrite(PIN_, MODE_OFF_);
    }

  private:
    const I2CManager& I2C_MANAGER_;
    const uint8_t PIN_;
    const int MODE_ON_;
    const int MODE_OFF_;
};
}  // namespace module

#endif
