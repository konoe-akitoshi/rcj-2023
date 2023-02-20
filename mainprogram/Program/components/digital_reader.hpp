#ifndef COMPONENTS_DIGITAL_READER_HPP
#define COMPONENTS_DIGITAL_READER_HPP

#ifdef LOCAL_INCLUDE
#include "../../local/arduino_deps.hpp"
#include "../../local/Adafruit_PCF8574.hpp"
#else
#include <Adafruit_PCF8574.h>
#endif

#include "setup_handler.hpp"

namespace component
{
class DigitalReader
{
  public:
    /**
     * @param pin pin number.
     * @param mode pin mode. e.g. INPUT
     */
    explicit DigitalReader(SetupHandler& handler, const int pin, const int mode);

    /**
     * Check whether the pin is HIGH.
     *
     * @returns ture: pin value is HIGH.
     */
    bool IsHigh() const;

    /**
     * Check whether the pin is LOW.
     *
     * @returns ture: pin value is LOW.
     */
    bool IsLow() const;

  private:
    const int PIN_;
};

class DigitalReaderPCF8575
{
  public:
    explicit DigitalReaderPCF8575(SetupHandler& handler, Adafruit_PCF8574& pcf8574, const int pin, const int mode);

    /**
     * Check whether the pin is HIGH.
     *
     * @returns ture: pin value is HIGH.
     */
    bool IsHigh() const;

    /**
     * Check whether the pin is LOW.
     *
     * @returns ture: pin value is LOW.
     */
    bool IsLow() const;

  private:
    Adafruit_PCF8574& pcf8574_;
    const int PIN_;
};

inline DigitalReader::DigitalReader(SetupHandler& handler, const int pin, const int mode) : PIN_(pin) {
    handler.SetPinMode(PIN_, mode);
}

inline bool DigitalReader::IsHigh() const {
    return digitalRead(PIN_) == HIGH;
}

inline bool DigitalReader::IsLow() const {
    return digitalRead(PIN_) == LOW;
}

inline DigitalReaderPCF8575::DigitalReaderPCF8575(SetupHandler& handler, Adafruit_PCF8574& pcf8574, const int pin, const int mode)
    : pcf8574_(pcf8574), PIN_(pin){
    handler.SetCallback([this, mode]() {
        pcf8574_.pinMode(PIN_, mode);
    });
}

inline bool DigitalReaderPCF8575::IsHigh() const {
    return pcf8574_.digitalRead(PIN_) == HIGH;
}

inline bool DigitalReaderPCF8575::IsLow() const {
    return pcf8574_.digitalRead(PIN_) == LOW;
}

}  // namespace component

#endif
