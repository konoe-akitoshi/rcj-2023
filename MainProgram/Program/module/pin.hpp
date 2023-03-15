#ifndef MODULE_PIN_HPP
#define MODULE_PIN_HPP

#ifdef WITHOUT_ARDUINO_ENVIRONMENT
#include "core/deps/arduino.hpp"
#endif

namespace raspberry_pi_pico
{
constexpr pin_size_t PIN01_GP00 = 0;
constexpr pin_size_t PIN02_GP01 = 1;
constexpr pin_size_t PIN04_GP02 = 2;
constexpr pin_size_t PIN05_GP03 = 3;
constexpr pin_size_t PIN06_GP04 = 4;
constexpr pin_size_t PIN07_GP05 = 5;
constexpr pin_size_t PIN09_GP06 = 6;
constexpr pin_size_t PIN10_GP07 = 7;
constexpr pin_size_t PIN11_GP08 = 8;
constexpr pin_size_t PIN12_GP09 = 9;
constexpr pin_size_t PIN14_GP10 = 10;
constexpr pin_size_t PIN15_GP11 = 11;
constexpr pin_size_t PIN16_GP12 = 12;
constexpr pin_size_t PIN17_GP13 = 13;
constexpr pin_size_t PIN19_GP14 = 14;
constexpr pin_size_t PIN20_GP15 = 25;
constexpr pin_size_t PIN21_GP16 = 16;
constexpr pin_size_t PIN22_GP17 = 17;
constexpr pin_size_t PIN24_GP18 = 18;
constexpr pin_size_t PIN25_GP19 = 19;
constexpr pin_size_t PIN26_GP20 = 20;
constexpr pin_size_t PIN27_GP21 = 21;
constexpr pin_size_t PIN29_GP22 = 22;
constexpr pin_size_t PIN31_GP26 = 26;
constexpr pin_size_t PIN32_GP27 = 27;
constexpr pin_size_t PIN34_GP28 = 28;
}  // namespace raspberry_pi_pico

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

#endif
