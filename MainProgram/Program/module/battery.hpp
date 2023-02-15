#ifndef MODULE_BATTERY_HPP
#define MODULE_BATTERY_HPP

#include "_pin.hpp"
#include "core/battery.hpp"

// Low limit voltage 1.1*12 = 13.2
// Mi-NH なら 13.0, Li-po なら 13.5 (Li-po は過放電するので注意！)
const module::Battery Battery(raspberry_pi_pico::PIN32_GP27, 13.0);

#endif
