#ifndef MODULE_SWITCH_HPP
#define MODULE_SWITCH_HPP

#include "pin.hpp"
#include "core/digital_reader.hpp"

constexpr module::DigitalReader StartSwitch(pins::rp2040::GPIO02, INPUT_PULLUP);
constexpr module::DigitalReader GoalSwitch(pins::rp2040::GPIO03, INPUT_PULLUP);

#endif
