#ifndef MODULE_CORE_DIGITAL_READER_HPP
#define MODULE_CORE_DIGITAL_READER_HPP

#ifdef WITHOUT_ARDUINO_ENVIRONMENT
#include "deps/Adafruit_PCF8574.hpp"
#include "deps/arduino.hpp"
#else
#include <Adafruit_PCF8574.h>
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
    explicit constexpr DigitalReaderPCF8575(Adafruit_PCF8574& pcf8574, const uint8_t pin, const int mode)
        : pcf8574_(pcf8574), PIN_(pin), MODE_(mode) {
    }

    void setup(void) const {
        pcf8574_.pinMode(PIN_, MODE_);
    }

    /**
     * Check whether the pin is HIGH.
     *
     * @returns ture: pin value is HIGH.
     */
    bool isHigh(void) const {
        return pcf8574_.digitalRead(PIN_) == HIGH;
    }

    /**
     * Check whether the pin is LOW.
     *
     * @returns ture: pin value is LOW.
     */
    bool isLow() const {
        return pcf8574_.digitalRead(PIN_) == LOW;
    }

  private:
    Adafruit_PCF8574& pcf8574_;
    const uint8_t PIN_;
    const int MODE_;
};
}  // namespace module

#endif
