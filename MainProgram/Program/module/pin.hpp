#ifndef MODULE_PIN_HPP
#define MODULE_PIN_HPP

#ifdef WITHOUT_ARDUINO_ENVIRONMENT
#include "core/deps/arduino.hpp"
#endif

namespace pins
{
namespace rp2040
{
constexpr pin_size_t GPIO00 = 0;
constexpr pin_size_t GPIO01 = 1;
constexpr pin_size_t GPIO02 = 2;
constexpr pin_size_t GPIO03 = 3;
constexpr pin_size_t GPIO04 = 4;
constexpr pin_size_t GPIO05 = 5;
constexpr pin_size_t GPIO06 = 6;
constexpr pin_size_t GPIO07 = 7;
constexpr pin_size_t GPIO08 = 8;
constexpr pin_size_t GPIO09 = 9;
constexpr pin_size_t GPIO10 = 10;
constexpr pin_size_t GPIO11 = 11;
constexpr pin_size_t GPIO12 = 12;
constexpr pin_size_t GPIO13 = 13;
constexpr pin_size_t GPIO14 = 14;
constexpr pin_size_t GPIO15 = 15;
constexpr pin_size_t GPIO16 = 16;
constexpr pin_size_t GPIO17 = 17;
constexpr pin_size_t GPIO18 = 18;
constexpr pin_size_t GPIO19 = 19;
constexpr pin_size_t GPIO20 = 20;
constexpr pin_size_t GPIO21 = 21;
constexpr pin_size_t GPIO22 = 22;
constexpr pin_size_t GPIO23 = 23;
constexpr pin_size_t GPIO24 = 24;
constexpr pin_size_t GPIO25 = 25;
constexpr pin_size_t GPIO26 = 26;
constexpr pin_size_t GPIO27 = 27;
constexpr pin_size_t GPIO28 = 28;
constexpr pin_size_t GPIO29 = 29;
}  // namespace rp2040

namespace pcf8574
{
constexpr uint8_t EX00 = 0;
constexpr uint8_t EX01 = 1;
constexpr uint8_t EX02 = 2;
constexpr uint8_t EX03 = 3;
constexpr uint8_t EX04 = 4;
constexpr uint8_t EX05 = 5;
constexpr uint8_t EX06 = 6;
constexpr uint8_t EX07 = 7;
}  // namespace pcf8574
}  // namespace pins

#endif
