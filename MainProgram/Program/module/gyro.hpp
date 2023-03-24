#ifndef MODULE_GYRO_HPP
#define MODULE_GYRO_HPP

#include "core/gyro.hpp"
#include "pin.hpp"

constexpr module::Gyro Gyro(115200, pins::rp2040::GPIO23, pins::rp2040::GPIO07);

#endif
