#ifndef MODULE_LED_HPP
#define MODULE_LED_HPP

#include "core/led_light.hpp"
#include "i2c_manager.hpp"
#include "pin.hpp"

constexpr module::LedLightPCF8574 LedR(I2CManager, pins::pcf8574::EX07, LOW, HIGH);
constexpr module::LedLightPCF8574 LedY(I2CManager, pins::pcf8574::EX05, LOW, HIGH);
constexpr module::LedLightPCF8574 LedG(I2CManager, pins::pcf8574::EX06, LOW, HIGH);
constexpr module::LedLightPCF8574 LedB(I2CManager, pins::pcf8574::EX04, LOW, HIGH);

constexpr module::LedLight SwitchLedR(pins::rp2040::GPIO14);
constexpr module::LedLight SwitchLedG(pins::rp2040::GPIO15);
constexpr module::LedLight LineSensorLed(pins::rp2040::GPIO22);

#endif
