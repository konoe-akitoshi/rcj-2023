#ifndef MODULE_LED_HPP
#define MODULE_LED_HPP

#include "core/led_light.hpp"
#include "_pin.hpp"

Adafruit_PCF8574 PCF8574;

const module::LedLightPCF8574 LedR(PCF8574, pcf8574::EX07, LOW, HIGH);
const module::LedLightPCF8574 LedY(PCF8574, pcf8574::EX05, LOW, HIGH);
const module::LedLightPCF8574 LedG(PCF8574, pcf8574::EX06, LOW, HIGH);
const module::LedLightPCF8574 LedB(PCF8574, pcf8574::EX04, LOW, HIGH);

const module::LedLightPCF8574 SwitchLedR(PCF8574, pcf8574::EX02);
const module::LedLightPCF8574 SwitchLedG(PCF8574, pcf8574::EX03);

const module::LedLight LineSensorLed(raspberry_pi_pico::PIN27_GP21);

#endif
