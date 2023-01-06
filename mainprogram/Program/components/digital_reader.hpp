#ifndef COMPONENTS_DIGITAL_READER_HPP
#define COMPONENTS_DIGITAL_READER_HPP

#ifdef LOCAL_INCLUDE
#include "../../local/arduino_deps.hpp"
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

inline DigitalReader::DigitalReader(SetupHandler& handler, const int pin, const int mode) : PIN_(pin) {
    handler.SetPinMode(PIN_, mode);
}

inline bool DigitalReader::IsHigh() const {
    return digitalRead(PIN_) == HIGH;
}

inline bool DigitalReader::IsLow() const {
    return digitalRead(PIN_) == LOW;
}
}  // namespace component

#endif
