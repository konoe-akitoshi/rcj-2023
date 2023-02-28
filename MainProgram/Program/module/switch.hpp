#ifndef MODULE_SWITCH_HPP
#define MODULE_SWITCH_HPP

#include "pin.hpp"
#include "core/digital_reader.hpp"

constexpr module::DigitalReader StartSwitch(raspberry_pi_pico::PIN04_GP02, INPUT_PULLUP);
constexpr module::DigitalReader GoalSwitch(raspberry_pi_pico::PIN05_GP03, INPUT_PULLUP);

#endif
