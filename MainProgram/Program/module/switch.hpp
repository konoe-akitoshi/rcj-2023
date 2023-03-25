#ifndef MODULE_SWITCH_HPP
#define MODULE_SWITCH_HPP

#include "i2c_manager.hpp"
#include "pin.hpp"
#include "core/digital_reader.hpp"

constexpr module::DigitalReader StartSwitch(pins::rp2040::GPIO02, INPUT_PULLUP);
constexpr module::DigitalReader GoalSwitch(pins::rp2040::GPIO03, INPUT_PULLUP);

constexpr module::DigitalReaderPCF8575 AttackerSwitch(I2CManager, pins::pcf8574::EX00, INPUT_PULLUP);
constexpr module::DigitalReaderPCF8575 KeeperSwitch(I2CManager, pins::pcf8574::EX01, INPUT_PULLUP);

#endif
