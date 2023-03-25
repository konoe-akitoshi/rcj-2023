#ifndef MODULE_CORE_DIGITAL_READER_HPP
#define MODULE_CORE_DIGITAL_READER_HPP

#include "i2c_manager.hpp"

#ifdef WITHOUT_ARDUINO_ENVIRONMENT
#include "deps/arduino.hpp"
#else
#include "Arduino.h"
#endif

namespace module
{
class DigitalReader
{
  public:
    /**
     * @param pin pin number.
     * @param mode pin mode. e.g. INPUT
     */
    explicit constexpr DigitalReader(const pin_size_t pin, const int mode) : PIN_(pin), MODE_(mode) {
    }

    /**
     * Set up the digital-reader.
     */
    void setup(void) const {
        pinMode(PIN_, MODE_);
    }

    /**
     * Check whether the pin is HIGH.
     *
     * @returns ture: pin value is HIGH.
     */
    bool isHigh(void) const {
        return digitalRead(PIN_) == HIGH;
    }

    /**
     * Check whether the pin is LOW.
     *
     * @returns ture: pin value is LOW.
     */
    bool isLow(void) const {
        return digitalRead(PIN_) == LOW;
    }

  private:
    const pin_size_t PIN_;
    const int MODE_;
};

class DigitalReaderPCF8575
{
  public:
    explicit constexpr DigitalReaderPCF8575(const I2CManager& i2c_manager, const uint8_t pin, const int mode)
        : I2C_MANAGER_(i2c_manager), PIN_(pin), MODE_(mode) {
    }

    void setup(void) const {
        I2C_MANAGER_.beginPCF8574();
        I2C_MANAGER_.pcf8574_pinMode(PIN_, MODE_);
    }

    /**
     * Check whether the pin is HIGH.
     *
     * @returns ture: pin value is HIGH.
     */
    bool isHigh(void) const {
        return I2C_MANAGER_.pcf8574_digitalRead(PIN_);
    }

    /**
     * Check whether the pin is LOW.
     *
     * @returns ture: pin value is LOW.
     */
    bool isLow() const {
        return !I2C_MANAGER_.pcf8574_digitalRead(PIN_);
    }

  private:
    const I2CManager& I2C_MANAGER_;
    const uint8_t PIN_;
    const int MODE_;
};
}  // namespace module

#endif
