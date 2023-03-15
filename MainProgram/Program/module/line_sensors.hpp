#ifndef MODULE_LINE_SENSOR_HPP
#define MODULE_LINE_SENSOR_HPP

#include "pin.hpp"
#include "core/line_sensors.hpp"

constexpr module::LineSensors LineSensors(
    pins::rp2040::GPIO28,  // interrupt
    pins::rp2040::GPIO16,  // front sensor
    pins::rp2040::GPIO17,  // right
    pins::rp2040::GPIO18,  // back
    pins::rp2040::GPIO19,  // left
    pins::rp2040::GPIO20   // center
);

#endif
