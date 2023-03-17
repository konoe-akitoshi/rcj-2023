#ifndef MODULE_I2C_MANAGER_HPP
#define MODULE_I2C_MANAGER_HPP

#include "core/i2c_manager.hpp"
#include "pin.hpp"

Adafruit_PCF8574 PCF8574;

constexpr module::I2CManager I2CManager(
    pins::rp2040::GPIO04, pins::rp2040::GPIO05, 400000,  // Wire
    pins::rp2040::GPIO10, pins::rp2040::GPIO11,          // Wire1
    PCF8574, 0x27                                        // PCF8574
);

#endif
