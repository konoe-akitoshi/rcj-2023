#ifndef MODULE_LINE_SENSOR_HPP
#define MODULE_LINE_SENSOR_HPP

#include "pin.hpp"
#include "core/line_sensors.hpp"

constexpr module::LineSensors LineSensors(
    raspberry_pi_pico::PIN34_GP28,  // interrupt
    raspberry_pi_pico::PIN21_GP16,  // front sensor
    raspberry_pi_pico::PIN22_GP17,  // right
    raspberry_pi_pico::PIN24_GP18,  // back
    raspberry_pi_pico::PIN25_GP19,  // left
    raspberry_pi_pico::PIN26_GP20   // center
);

#endif
