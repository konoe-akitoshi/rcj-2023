#ifndef COMPONENT_DIGITAL_READER_HPP
#define COMPONENT_DIGITAL_READER_HPP

#ifdef LOCAL_INCLUDE
#include "../../local/arduino_deps.h"
#endif

namespace component
{
class DigitalReader
{
  public:
    explicit DigitalReader(const int pin, const int mode);
    bool IsHigh() const;
    bool IsLow() const;

  private:
    const int PIN_;
};

inline DigitalReader::DigitalReader(const int pin, const int mode) : PIN_(pin) {
    pinMode(PIN_, mode);
}

inline bool DigitalReader::IsHigh() const {
    return digitalRead(PIN_) == HIGH;
}

inline bool DigitalReader::IsLow() const {
    return digitalRead(PIN_) == LOW;
}
}  // namespace component

#endif
