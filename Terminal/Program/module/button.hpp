#ifndef MODULE_BUTTON_HPP
#define MODULE_BUTTON_HPP

#include "core/button.hpp"

#ifdef WITHOUT_ARDUINO_ENVIRONMENT
#include "core/deps/arduino.hpp"
#else
#include "Arduino.h"
#endif

constexpr module::Button ButtonRight(WIO_KEY_A);
constexpr module::Button ButtonCenter(WIO_KEY_B);
constexpr module::Button ButtonLeft(WIO_KEY_C);

constexpr module::Button StickUp(WIO_5S_UP);
constexpr module::Button StickDown(WIO_5S_DOWN);
constexpr module::Button StickRight(WIO_5S_RIGHT);
constexpr module::Button StickLeft(WIO_5S_LEFT);
constexpr module::Button StickPress(WIO_5S_PRESS);

#endif
