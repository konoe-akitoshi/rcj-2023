#ifndef COMPONENT_KICKER_HPP
#define COMPONENT_KICKER_HPP

#ifdef LOCAL_INCLUDE
#include "../../local/arduino_deps.h"
#endif

namespace component
{
class Kicker
{
  public:
    explicit Kicker(const int pin);
    void PushFront() const;
    void PullBack() const;

  private:
    const int PIN_;
};

inline Kicker::Kicker(const int pin) : PIN_(pin) {
    pinMode(PIN_, OUTPUT);
    digitalWrite(PIN_, LOW);
}

inline void Kicker::PushFront() const {
    digitalWrite(PIN_, HIGH);
}
inline void Kicker::PullBack() const {
    digitalWrite(PIN_, LOW);
}
}  // namespace component

#endif
